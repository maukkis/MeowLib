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
  int shard = ready.shardId;

  shards.at(shard).api.sesId = j["session_id"];
  shards.at(shard).api.resumeUrl = j["resume_gateway_url"];
  user.cache.insert(ready.user.id, ready.user);
  user.cache.setCurrentUser(ready.user);
  if(shards.at(shard).api.resumeUrl.back() != '/')
    shards.at(shard).api.resumeUrl.append("/?v=10&encoding=json");
  else shards.at(shard).api.resumeUrl.append("?v=10&encoding=json");
  if(shard == 0){
    std::unique_lock<std::mutex> lock(api.UnavailableGuildIdsmtx);
    for(const auto& a : j["guilds"]){
      ready.guilds.emplace_back(UnavailableGuild{.id = a["id"], .unavailable = a["unavailable"]});
      api.unavailableGuildIds.insert(a["id"]);
    }
  }
  api.appId = ready.user.id;
  shards.at(shard).api.state = GatewayStates::READY;
  if(funs.onReadyF && shard == 0)
    funs.onReadyF(ready);
}

void NyaBot::resumed(nlohmann::json j){
  Log::info("connection resumed with sequence " + std::to_string(j["s"].get<long>()));
}


