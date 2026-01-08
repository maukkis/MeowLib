#ifndef _INCLUDE_VOICE_H
#define _INCLUDE_VOICE_H
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

struct VoiceCallbacks {
  VoiceCallbacks() = default;
  std::mutex mtx;
  VoiceInfo info;
  std::optional<std::string> channelId = std::nullopt;
  bool ready(){
    return !info.endpoint.empty() && !info.guildId.empty()
            && !info.token.empty() && !info.sessionId.empty();
  }
  std::function<void(bool)> closeCallback;
  std::function<void(VoiceInfo&)> voiceServerUpdate;
  VoiceCallbacks& operator=([[maybe_unused]]const VoiceCallbacks& a){return *this;}
};



class VoiceApiRoutes {
public:
  VoiceApiRoutes(NyaBot *bot);
  std::expected<VoiceState, Error> getUserVoiceState(const std::string_view guildId, const std::string_view userId);
private:
  NyaBot *bot;
};


#endif
