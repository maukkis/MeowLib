#include "../include/shard.h"
#include "../include/nyaBot.h"
#include "../include/eventCodes.h"
#include <nlohmann/json.hpp>

#ifndef OS
#define OS "unknown"
#endif


Shard::Shard(NyaBot *bot, int shardId, bool noShard) : bot{bot}{
  api.noShard = noShard;
  api.shardId = shardId;
  handle.setUrl(bot->api.defaultUrl);
  if(auto res = connect(); !res){
    Log::error("we failed to connect to the gateway due to: " + std::to_string(res.error()));
    return;
  }
  getHeartbeatInterval();
  sendIdent();
  th = std::thread(&Shard::listen, this);
}


Shard::~Shard(){
  bot->stop = true;
  if(th.joinable()) th.join();
}



GatewayStates Shard::getState(){
  return api.state;
}


int NyaBot::getNumShards(){
  return api.numShards;
}

void Shard::routeEvent(const nlohmann::json& j){
  std::string meow = j["t"];
  if(meow == "RESUMED"){
    api.state = GatewayStates::READY;
  }
  if(bot->dispatchHandlers.contains(meow)){
    std::thread{bot->dispatchHandlers[meow], std::move(j)}.detach();
  }
  else {
    Log::warn("unknown dispatch");
    Log::dbg(j.dump());
  }
}

meow Shard::reconnect(bool resume){
  api.state = GatewayStates::UNAUTHENTICATED;
  if(handle.wsClose(1012, "arf") != OK){
    Log::dbg("woof?");
  }

  if(api.resumeUrl.find("wss") != std::string::npos)
    api.resumeUrl.replace(0, 3, "https");

  handle.setUrl(resume ? api.resumeUrl : bot->api.defaultUrl);
  if(!connect()){
    Log::error("failed to connect to the gateway");
    bot->stop = true;
    return meow::ERR_CONNECT_FAILED;
  }
  getHeartbeatInterval();
  if(resume){
    nlohmann::json j;
    j["op"] = Resume;
    j["d"]["token"] = bot->api.token;
    j["d"]["session_id"] = api.sesId;
    j["d"]["seq"] = api.sequence;
    if(handle.wsSend(j.dump(), meowWs::meowWS_TEXT) > 0 ){
      Log::dbg("sent resume!");
      api.state = GatewayStates::RESUME_SENT;
    }
  } else {
    sendIdent();
  }
  return OK;
}


std::expected<std::nullopt_t, meow> Shard::connect(){
  int timeToWait = 5;
  meow mraow{};
  for(int attempts = 0; attempts < bot->retryAmount && !bot->stop; ++attempts, timeToWait *= 2){
    if(mraow = handle.perform(); mraow == OK){
      Log::dbg("connected to the websocket succesfully on shard " + std::to_string(api.shardId));
      return std::nullopt;
    }
    else {
      if(timeToWait > 300) timeToWait = 300;
      Log::error("failed to connect to the gateway waiting " + std::to_string(timeToWait) + " seconds");
      std::this_thread::sleep_for(std::chrono::seconds(timeToWait));
    }
  }
  return std::unexpected(mraow);
}

void Shard::sendIdent(){
  Log::dbg("sending ident");
  nlohmann::json j;
  j["op"] = Identify;
  nlohmann::json d;
  d["token"] = bot->api.token;
  d["intents"] = bot->api.intents;
  if(!api.noShard)
    d["shard"] = {api.shardId, bot->api.numShards};
  if(bot->presence)
    d["presence"] = serializePresence(*bot->presence);
  nlohmann::json p;
  p["os"] = OS;
  p["browser"] = "MeowLib";
  p["device"] = "MeowLib";
  d["properties"] = std::move(p);
  j["d"] = std::move(d);
  if (handle.wsSend(j.dump(), meowWs::meowWS_TEXT) > 0){
    Log::dbg("ident sent on shard: " + std::to_string(api.shardId));
    api.state = GatewayStates::AUTHENTICATION_SENT;
  }
  else {
    Log::error("something went wrong");
  }
}

void Shard::getHeartbeatInterval(){
  std::string buf;
  try{
    meowWs::meowWsFrame frame;
    handle.wsRecv(buf, &frame);
    auto meowJson = nlohmann::json::parse(buf);
    api.interval = meowJson["d"]["heartbeat_interval"];
  }
  catch(nlohmann::json::exception& e){
    Log::error("failed to get the heartbeat interval");
    Log::dbg("buffer is " + buf);
    bot->stop = true;
  }

}



void Shard::close(){
  handle.wsClose(1000, "arff *tail wags*");
}



void NyaBot::globalSend(const std::string& payload){
  for(auto& shard : shards){
    shard.queue.addToQueue(payload);
  }
}
