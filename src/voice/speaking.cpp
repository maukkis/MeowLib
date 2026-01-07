#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <meowHttp/websocket.h>
#include <mutex>
#include <thread>
#include <utility>



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
  auto lastSent = std::chrono::high_resolution_clock::now();
  while(!api.stop){
    std::unique_lock<std::mutex> lock(qmtx);
    if(voiceDataQueue.empty()) fcv.notify_all();
    qcv.wait(lock, [this](){
      return (!voiceDataQueue.empty() && !udpInterrupt) || api.stop;
    });
    if(api.stop) return;
    auto a = std::move(voiceDataQueue.front());
    voiceDataQueue.pop_front();
    lock.unlock();
    #ifdef WIN32
    int slen = sendto(uSockfd, std::bit_cast<char*>(a.payload.data()), a.payloadLen, 0, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest)); // stupid
    #else
    int slen = sendto(uSockfd, a.payload.data(), a.payloadLen, 0, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest));
    #endif
    if(slen <= 0){
      Log::dbg("couldnt send");
      continue;
    }
    auto time = std::chrono::high_resolution_clock::now() - lastSent;
    auto tts = std::chrono::nanoseconds(
      a.duration * msToNs
      - std::chrono::duration_cast<std::chrono::nanoseconds>(time).count()
    );
    if(tts <= std::chrono::nanoseconds(0)){
      Log::dbg("bite");
      tts = std::chrono::nanoseconds(a.duration * msToNs);
    }
    std::this_thread::sleep_for(tts);
    lastSent = std::chrono::high_resolution_clock::now();
  }
}


void VoiceConnection::sendSpeaking(){
  nlohmann::json j;
  j["op"] = VoiceOpcodes::SPEAKING;
  j["d"]["speaking"] = MICROPHONE;
  j["d"]["delay"] = 0;
  j["d"]["ssrc"] = api.ssrc;
  handle.wsSend(j.dump(), meowWs::meowWS_TEXT);
  Log::dbg("speaking sent");
}





void VoiceConnection::sendSilence(){
  // we need exclusive access to the queue
  std::unique_lock<std::mutex> lock(qmtx);
  if(!voiceDataQueue.empty()){
    auto item = voiceDataQueue.front();
    voiceDataQueue.clear();
    rtpHeader a;
    std::memcpy(&a, item.payload.data(), sizeof(a));
    api.rtpSeq = ntohs(a.seq);
    api.timestamp = ntohl(a.ts);
    assert(ntohl(a.ssrc) == api.ssrc && "ssrcs arent the same");
  }
  for(size_t i = 0; i < 5; ++i){
    Log::dbg("sending silence");
    auto a = frameRtp(silence, frameSize);
    if(a.second == 0) return;
    #ifdef WIN32
    // WHY TF IS THIS MARKED NO DISCARD
    [[maybe_unused]] int slen = sendto(uSockfd, std::bit_cast<char*>(a.first.data()), a.second, 0, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest)); // stupid
    #else
    sendto(uSockfd, a.first.data(), a.second, 0, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest));
    #endif
    std::this_thread::sleep_for(std::chrono::milliseconds(frameSize / 48));
  } 
}


