#include "../../include/nyaBot.h"
#include <mutex>
#include <string>

Ready::Ready(const nlohmann::json& j){
  v = j["v"];
  user = User(j["user"]);
  if(j.contains("shard")){
    shardId = j["shard"].at(0);
    numShards = j["shard"].at(1);
  }
}

void NyaBot::ready(nlohmann::json j){
  j = j["d"];
  Ready ready(j);
  api.sesId = j["session_id"];
  api.resumeUrl = j["resume_gateway_url"];
  user.cache.insert(ready.user.id, ready.user);
  user.cache.setCurrentUser(ready.user);
  if(api.resumeUrl.back() != '/')
    api.resumeUrl.append("/?v=10&encoding=json");
  else api.resumeUrl.append("?v=10&encoding=json");
  std::unique_lock<std::mutex> lock(api.UnavailableGuildIdsmtx); 
  for(const auto& a : j["guilds"]){
    ready.guilds.emplace_back(UnavailableGuild{.id = a["id"], .unavailable = a["unavailable"]});
    api.unavailableGuildIds.insert(a["id"]);
  }
  lock.unlock();
  api.appId = ready.user.id;
  api.state = GatewayStates::READY;
  if(funs.onReadyF)
    funs.onReadyF(ready);
}

void NyaBot::resumed(nlohmann::json j){
  api.state = GatewayStates::READY;
  Log::info("connection resumed with sequence " + std::to_string(j["s"].get<long>()));
}


