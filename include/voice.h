#ifndef _INCLUDE_VOICE_H
#define _INCLUDE_VOICE_H
#include <coroutine>
#include <optional>
#include <string>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <expected>
#include "error.h"


class NyaBot;
//these exist here and will get included to prevent compile errors they dont contain anything special so they can be included 
//this file will also later have VoiceState struct and api routes
struct VoiceInfo {
  std::string endpoint;
  std::string guildId;
  std::string token;
  std::string sessionId;
};

struct VoiceState {
  VoiceState() = default;
  VoiceState(const nlohmann::json& j);
  std::optional<std::string> guildId = std::nullopt;
  std::optional<std::string> channelId = std::nullopt;
  std::string userId;
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

class VoiceApiRoutes {
public:
  VoiceApiRoutes(NyaBot *bot);
  std::expected<VoiceState, Error> getUserVoiceState(const std::string_view guildId, const std::string_view userId);
private:
  NyaBot *bot;
};


#endif
