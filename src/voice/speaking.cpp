#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <cstdint>
#include <meowHttp/websocket.h>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>
#include <utility>

void VoiceConnection::sendSpeaking(){
  nlohmann::json j;
  j["op"] = VoiceOpcodes::SPEAKING;
  j["d"]["speaking"] = 5;
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
  int dur = 48 * duration;
  std::vector<uint8_t> vec;
  for(size_t i = 0; i < frameSize; ++i){
    vec.push_back(opusData[i]);
  }
  voiceDataQueue.addToQueue(std::make_pair(vec, dur));
}


std::pair<std::vector<uint8_t>, uint32_t> VoiceConnection::frameRtp(std::vector<uint8_t> a){
  static bool barkk = false;
  if(!barkk) sendSpeaking();
  barkk = true;
  std::vector<uint8_t> frame(a.size() + 12 + 4 + 16);
  rtpHeader rtp(api.rtpSeq, api.timestamp, api.ssrc);
  std::memcpy(frame.data(), &rtp, sizeof(rtp));
  for(size_t i = 0; i < sizeof(rtp); ++i){
    std::print("0x{:x} ", frame[i]);
  }
  std::println();
  std::vector<std::uint8_t> encryptedOpus(a.size() + 16);
  std::array<uint8_t, 12> nonce{0};
  std::memcpy(nonce.data(), &api.pNonce, sizeof(api.pNonce));
  int len = aeadAes256GcmRtpsizeEncrypt(a.data(),
                                        a.size(),
                                        api.secretKey.data(),
                                        nonce.data(),
                                        (uint8_t*)&rtp,
                                        sizeof(rtp),
                                        encryptedOpus.data());
  Log::dbg(std::to_string(len));
  if(len == -1){
    Log::error("something went wrong with encrypting");
    close();
  }
  uint32_t noncebe = htonl(api.pNonce);
  std::memcpy(frame.data() + sizeof(rtp), encryptedOpus.data(), len);
  std::memcpy(frame.data() + sizeof(rtp) + len, &noncebe, sizeof(noncebe));
  return {frame, 4 + 12 + len};
}
