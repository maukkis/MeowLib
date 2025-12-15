#include "../../include/nyaBot.h"
#include "../../include/voice/voiceconnection.h"

void setSessionId(std::unordered_map<std::string, VoiceTask>& arf, const nlohmann::json& j);

void NyaBot::voiceStateUpdate(nlohmann::json j){
  Log::dbg("arf!!");
  if(j["d"]["user_id"] != api.appId) return; // bark at you
  setSessionId(voiceTaskList, j);
}
