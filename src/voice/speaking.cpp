#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <meowHttp/websocket.h>
#include <mutex>
#include <sys/socket.h>
#include <thread>
#include <utility>

#ifdef WIN32
#define poll WSAPoll
#else
#include <sys/poll.h>
#endif

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
      return ((!voiceDataQueue.empty() || !sendDataQueue.empty()) && !udpInterrupt) || api.stop;
    });
    if(api.stop) return;

    if(sendDataQueue.empty()){
      auto b = std::move(voiceDataQueue.front());
      voiceDataQueue.pop_front();
      auto [data, len] = frameRtp(b.payload, b.samples);
      sendDataQueue.emplace_front(
        VoiceData{
          .payload = std::move(data),
          .payloadLen = len,
          .duration = b.duration,
          .samples = b.samples,
        });
    }
    lock.unlock();
    auto a = std::move(sendDataQueue.front());
    sendDataQueue.pop_front();
    pollfd pfd;
    pfd.fd = uSockfd;
    pfd.events = POLLOUT;
    int ret = poll(&pfd, 1, a.duration);
    if(ret != 0 && ret > 0 && pfd.revents & POLLOUT){
      #ifdef WIN32
      int slen = sendto(uSockfd, std::bit_cast<char*>(a.payload.data()), a.payloadLen, 0, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest)); // stupid
      #else
      int slen = sendto(uSockfd, a.payload.data(), a.payloadLen, 0, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest));
      #endif
      if(slen <= 0){
        Log::dbg("couldnt send");
      }
    } else Log::dbg("poll timeout reached or error");
    // at this time as we have to wait anyway we should prepare the next thing to send
    if(!voiceDataQueue.empty() && !udpInterrupt){
      lock.lock();
      auto a = voiceDataQueue.front();
      voiceDataQueue.pop_front();
      lock.unlock();
      auto [data, len] = frameRtp(a.payload, a.samples);
      sendDataQueue.emplace_front(
        VoiceData{
          .payload = data,
          .payloadLen = len,
          .duration = a.duration,
          .samples = a.samples,
        });
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
  if(!sendDataQueue.empty()){
    auto item = sendDataQueue.front();
    sendDataQueue.clear();
    rtpHeader a;
    std::memcpy(&a, item.payload.data(), sizeof(a));
    api.rtpSeq = ntohs(a.seq);
    api.timestamp = ntohl(a.ts);
    assert(ntohl(a.ssrc) == api.ssrc && "ssrcs arent the same");
  }
  for(size_t i = 0; i < 5; ++i){
    Log::dbg("sending silence");
    auto a = frameRtp(silence, 960);
    if(a.second == 0) return;
    #ifdef WIN32
    // WHY TF IS THIS MARKED NO DISCARD
    [[maybe_unused]] int slen = sendto(uSockfd, std::bit_cast<char*>(a.first.data()), a.second, 0, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest)); // stupid
    #else
    sendto(uSockfd, a.first.data(), a.second, 0, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest));
    #endif
    std::this_thread::sleep_for(std::chrono::milliseconds(960 / 48));
  } 
}


