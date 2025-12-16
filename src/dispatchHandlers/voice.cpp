#include "../../include/nyaBot.h"
#include "../../include/voice/voiceconnection.h"

#ifndef VOICE_ENABLED 
// we have to have a stub for voice server update
void NyaBot::voiceServerUpdate([[maybe_unused]]nlohmann::json j){}
#define sessionIdFun(x, y)
#endif
#ifdef VOICE_ENABLED
void setSessionId(std::unordered_map<std::string, VoiceTask>& arf, const nlohmann::json& j);
#define sessionIdFun(x, y) setSessionId(x, y);
#endif

void NyaBot::voiceStateUpdate(nlohmann::json j){
  if(j["d"]["user_id"] != api.appId ||
     j["d"]["channel_id"].is_null()) return; // bark at you

  sessionIdFun(voiceTaskList, j);
}



// this is undocumented and exists as a placeholder
// fields
// id: channelId
// guild_id: guildId
// status: ?the voice channel status
void NyaBot::voiceChannelStatusUpdate([[maybe_unused]]nlohmann::json j){}

// this is undocumented and exists as a placeholder
// fields
// id: channelId
// guild_id: guildId
// voice_start_time: ?unix timestamp when the start time got started null if it ended
void NyaBot::voiceChannelStartTimeUpdate([[maybe_unused]]nlohmann::json j){}
