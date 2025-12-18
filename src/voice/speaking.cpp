#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <chrono>
#include <cstdint>
#include <meowHttp/websocket.h>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <utility>

constexpr int msToNs = 1000000;
constexpr int rtpFrameSize = 12;
constexpr int aes256GcmTagSize = 16;
constexpr int aes256GcmAADSize = 12;
constexpr int sampleRate = 48000;

enum SpeakingCodes : uint8_t {
  MICROPHONE = 1 << 0,
  SOUNDSHARD = 1 << 1,
  PRIORITY = 1 << 2,
};


void VoiceConnection::addToQueue(const VoiceData& a){
  std::unique_lock<std::mutex> lock(qmtx);
  voiceDataQueue.emplace_back(std::move(a));
  lock.unlock();
  qcv.notify_one();
}

void VoiceConnection::udpLoop(){
  while(!api.stop){ 
    std::unique_lock<std::mutex> lock(qmtx);
    qcv.wait(lock, [this](){
      return !voiceDataQueue.empty() || api.stop;
    });
    if(api.stop) return;
    auto a = voiceDataQueue.front();
    voiceDataQueue.pop_front();
    lock.unlock();
    auto start = std::chrono::high_resolution_clock::now();
    int slen = sendto(uSockfd, a.payload.data(), a.payloadLen, 0, (sockaddr*)&api.dest, sizeof(api.dest));
    auto end = std::chrono::high_resolution_clock::now();
    if(slen <= 0){
      Log::dbg("couldnt send");
    }
    auto time = end - start;
    auto tts = std::chrono::nanoseconds(
      a.duration * msToNs
      - std::chrono::duration_cast<std::chrono::nanoseconds>(time).count()
    );
    std::this_thread::sleep_for(tts);
  }
}


void VoiceConnection::sendSpeaking(){
  nlohmann::json j;
  j["op"] = VoiceOpcodes::SPEAKING;
  j["d"]["speaking"] = MICROPHONE;
  j["d"]["delay"] = 0;
  j["d"]["ssrc"] = api.ssrc;
  handle.wsSend(j.dump(), meowWs::meowWS_TEXT);
  api.speaking = true;
  Log::dbg("speaking sent");
}


struct rtpHeader {
  uint8_t magic1;
  uint8_t magic2;
  uint16_t seq;
  uint32_t ts;
  uint32_t ssrc;
  rtpHeader(uint16_t seq, uint32_t ts, uint32_t ssrc) : magic1(0x80), magic2(0x78), seq(htons(seq)), ts(htonl(ts)), ssrc(htonl(ssrc)) {}
};

void VoiceConnection::sendOpusData(uint8_t *opusData, uint64_t duration, uint64_t frameSize){
  // get the amount of samples per ms for the rtp timestamp
  int dur = (sampleRate / 1000) * duration;
  std::vector<uint8_t> vec;
  vec.reserve(frameSize);
  for(size_t i = 0; i < frameSize; ++i){
    vec.push_back(opusData[i]);
  }
  auto frame = frameRtp(std::move(vec), dur);
  addToQueue(
    VoiceData{
      .payload = std::move(frame.first),
      .payloadLen = frame.second,
      .duration = duration
    }
  );
}


std::pair<std::vector<uint8_t>, uint32_t> VoiceConnection::frameRtp(std::vector<uint8_t> a, int dur){
  std::vector<uint8_t> frame(a.size() + rtpFrameSize + sizeof(api.pNonce) + aes256GcmTagSize);

  rtpHeader rtp(api.rtpSeq, api.timestamp, api.ssrc);
  std::memcpy(frame.data(), &rtp, sizeof(rtp));

  std::vector<std::uint8_t> encryptedOpus(a.size() + aes256GcmTagSize);
  std::array<uint8_t, aes256GcmAADSize> nonce{0};

  std::memcpy(nonce.data(), &api.pNonce, sizeof(api.pNonce));
  // key = secretKey from discord
  // IV = pNonce padded by 8 null bytes
  // AAD = rtp frame
  // the nonce itself is appended to the end of the payload as big endian without any padding
  int len = aeadAes256GcmRtpsizeEncrypt(a.data(),
                                        a.size(),
                                        api.secretKey.data(),
                                        nonce.data(),
                                        (uint8_t*)&rtp,
                                        sizeof(rtp),
                                        encryptedOpus.data());
  if(len == -1){
    Log::error("something went wrong with encrypting");
    close();
  }
  uint32_t noncebe = htonl(api.pNonce++);
  std::memcpy(frame.data() + sizeof(rtp), encryptedOpus.data(), len);
  std::memcpy(frame.data() + sizeof(rtp) + len, &noncebe, sizeof(noncebe));
  ++api.rtpSeq;
  api.timestamp += dur;
  return {frame, sizeof(api.pNonce) + rtpFrameSize + len};
}
