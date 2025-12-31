#include "../../include/nyaBot.h"
#include "../../include/voice/voiceconnection.h"

void setSessionId(std::unordered_map<std::string, VoiceCallbacks>& arf, const nlohmann::json& j){
  if(!arf.contains(j["d"]["guild_id"])){
    Log::dbg("*bites you*");
    return;
  }
  
  auto& task = arf.at(j["d"]["guild_id"]);
  std::unique_lock<std::mutex> lock(task.mtx);
  task.info.sessionId = j["d"]["session_id"];
  lock.unlock();
  if(task.ready()) task.voiceServerUpdate(task.info);
}


void NyaBot::voiceServerUpdate(nlohmann::json j){
  if(!voiceTaskList.contains(j["d"]["guild_id"])){
    Log::dbg("*bites you*");
    return;
  }
  auto& task = voiceTaskList.at(j["d"]["guild_id"]);
  std::unique_lock<std::mutex> lock(task.mtx);
  task.info.token = j["d"]["token"];
  task.info.endpoint = j["d"]["endpoint"];
  task.info.guildId = j["d"]["guild_id"];
  lock.unlock();
  if(task.ready()) task.voiceServerUpdate(task.info);
}
