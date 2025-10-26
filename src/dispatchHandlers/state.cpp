#include "../../include/nyaBot.h"
#include <mutex>
#include <string>


void NyaBot::ready(nlohmann::json j){
  j = j["d"];
  api.sesId = j["session_id"];
  api.resumeUrl = j["resume_gateway_url"];
  if(api.resumeUrl.back() != '/')
    api.resumeUrl.append("/?v=10&encoding=json");
  else api.resumeUrl.append("?v=10&encoding=json");
  std::unique_lock<std::mutex> lock(api.UnavailableGuildIdsmtx); 
  for(const auto& a : j["guilds"]){
    api.unavailableGuildIds.insert(a["id"]);
  }
  lock.unlock();
  j = j["user"];
  api.appId = j["id"];
  api.state = GatewayStates::READY;
  if(funs.onReadyF)
    funs.onReadyF();
}

void NyaBot::resumed(nlohmann::json j){
  api.state = GatewayStates::READY;
  Log::info("connection resumed with sequence " + std::to_string(j["s"].get<long>()));
}


