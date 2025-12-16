#ifndef _INCLUDE_VOICE_H
#define _INCLUDE_VOICE_H
#include <coroutine>
#include <string>
#include <mutex>

//these exist here and will get included to prevent compile errors they dont contain anything special so they can be included 
//this file will also later have VoiceState struct and api routes
struct VoiceInfo {
  std::string endpoint;
  std::string guildId;
  std::string token;
  std::string sessionId;
};

struct VoiceTask {
  VoiceInfo info;
  std::mutex mtx;
  std::coroutine_handle<> hp;
  bool await_ready() const noexcept {
    return !info.endpoint.empty() && !info.guildId.empty()
            && !info.token.empty() && !info.sessionId.empty();
  }
  void await_suspend(std::coroutine_handle<> handle) noexcept {
    hp = handle;
  }
  VoiceTask& operator=([[maybe_unused]]const VoiceTask& a){return *this;}
  VoiceInfo await_resume() const noexcept {
    return info;
  }
};


#endif
