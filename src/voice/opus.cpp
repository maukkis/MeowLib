#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <cstdint>
#include <optional>
#include <opus/opus.h>
#include <opus/opus_defines.h>
#include <opus/opus_types.h>
#include <vector>

constexpr int channels = 2;
constexpr int pcmDataSize = frameSize * channels * sizeof(opus_int16);

std::expected<std::nullopt_t, int> VoiceConnection::sendPcmData(const uint8_t* pcmData, size_t len){
  if(api.stop) return std::unexpected(-1);
  std::vector<uint8_t> data(pcmData, pcmData + len);

  // pad it to fit into a frame
  if(data.size() % pcmDataSize != 0){
    data.resize(data.size() + pcmDataSize - (data.size() % pcmDataSize), 0);
  }
  int err = 0;
  OpusEncoder *enc = opus_encoder_create(sampleRate, channels, OPUS_APPLICATION_VOIP, &err);
  if(err != OPUS_OK){
    Log::error(std::string("failed to create an opus encoder ") + opus_strerror(err));
    return std::unexpected(err);
  }
  std::vector<std::vector<uint8_t>> frames;
  for(size_t i = 0; i < data.size() / pcmDataSize; ++i){
    const opus_int16 *d = std::bit_cast<const opus_int16*>(data.data() + i * pcmDataSize);
    std::vector<uint8_t> opusData(4000);
    int ret = opus_encode(enc, d, frameSize, opusData.data(), opusData.size());
    if(ret <= 0){
      Log::error(std::string("failed to encode opus") + opus_strerror(ret));
      opus_encoder_destroy(enc);
      return std::unexpected(ret);
    }
    opusData.resize(ret);
    frames.emplace_back(opusData);
  }
  opus_encoder_destroy(enc);
  for(auto& a : frames){
    int samples = opus_packet_get_nb_samples(a.data(), a.size(), sampleRate);
    sendOpusData(a.data(), samples / static_cast<int>(sampleRate / 1000), a.size());
  }
  return std::nullopt;
}


void VoiceConnection::sendOpusData(const uint8_t *opusData, uint64_t duration, uint64_t frameSize){
  while(api.state != VoiceGatewayState::READY && !api.stop); // stupid
  if(api.stop) return;
  // get the amount of samples per ms for the rtp timestamp
  int dur = (int)(sampleRate / 1000) * duration;
  std::vector<uint8_t> vec;
  vec.reserve(frameSize);
  for(size_t i = 0; i < frameSize; ++i){
    vec.push_back(opusData[i]);
  }
  addToQueue(
    VoiceData{
      .payload = std::move(vec),
      .payloadLen = vec.size(),
      .duration = duration,
      .samples = dur
    }
  );
}


std::pair<std::vector<uint8_t>, uint32_t> VoiceConnection::frameRtp(std::vector<uint8_t>& a, int dur){
  std::vector<uint8_t> opus;
  if(dave->ready()){
    opus = dave->encryptor->encrypt(a);
  } else {
    opus = a;
  }
  std::vector<uint8_t> frame(opus.size() + rtpFrameSize + sizeof(api.pNonce) + tagSize);
  rtpHeader rtp(api.rtpSeq, api.timestamp, api.ssrc);
  std::memcpy(frame.data(), &rtp, sizeof(rtp));
  auto [encryptedOpus, noncec, len] = transportEncrypt(opus.data(),
                                                       opus.size(),
                                                       api.secretKey.data(),
                                                       std::bit_cast<uint8_t *>(&rtp),
                                                       sizeof(rtp));
  std::memcpy(frame.data() + sizeof(rtp), encryptedOpus.data(), len);
  std::memcpy(frame.data() + sizeof(rtp) + len, &noncec, sizeof(noncec));
  ++api.rtpSeq;
  api.timestamp += dur;
  return {frame, sizeof(api.pNonce) + rtpFrameSize + len};
}
