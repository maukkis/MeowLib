#include "../include/voice.h"
#include "../include/helpers.h"
#include "../include/nyaBot.h"
#include <nlohmann/json.hpp>

VoiceState::VoiceState(const nlohmann::json& j){
  jsonToOptional(guildId, j, "guild_id");
  jsonToOptional(channelId, j, "channel_id");
  userId = j["user_id"];
}

VoiceApiRoutes::VoiceApiRoutes(NyaBot *bot) : bot{bot} {}

std::expected<VoiceState, Error> VoiceApiRoutes::getUserVoiceState(const std::string_view guildId,
                                                                   const std::string_view userId)
{
  auto res = bot->rest.get(std::format(APIURL "/guilds/{}/voice-states/{}", guildId, userId));
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to fetch user voice state");
    err.printErrors();
    return std::unexpected(err);
  }
  return VoiceState(nlohmann::json::parse(res->first));
}
