#include <meowHttp/websocket.h>
#include <csignal>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <thread>
#include "../include/eventCodes.h"
#include "../include/nyaBot.h"
#include <nlohmann/json.hpp>

NyaBot::NyaBot(int intents){
  api.intents = intents;
  a = this;
  std::signal(SIGINT, NyaBot::signalHandler);
  #ifndef NDEBUG
    Log::info(std::string("MeowLib version: ") + MEOWLIB_VERSION + " with " + std::to_string(dispatchHandlers.size()) + " gw events implemented");
  #endif
}

meow NyaBot::reconnect(bool resume){
  if(handle.wsClose(1012, "arf") != OK){
    Log::dbg("woof?");
  }

  if(api.resumeUrl.find("wss") != std::string::npos)
    api.resumeUrl.replace(0, 3, "https");

  handle.setUrl(resume ? api.resumeUrl : api.defaultUrl);
  connect();
  getHeartbeatInterval();
  if(resume){
    nlohmann::json j;
    j["op"] = Resume;
    j["d"]["token"] = api.token;
    j["d"]["session_id"] = api.sesId;
    j["d"]["seq"] = api.sequence;
    if(handle.wsSend(j.dump(), meowWs::meowWS_TEXT) > 0 ){
      Log::dbg("sent resume!");
    }
  } else {
    sendIdent();
  }
  return OK;
}

void NyaBot::run(const std::string_view token){
  api.token = token;
  Log::dbg("fetching /gateway/bot :3");
  auto res = rest.get(APIURL "/gateway/bot");
  if(!res.has_value() || res->second != 200){
    Log::error("failed to get /gateway/bot exiting :(");
    return;
  }
  auto j = nlohmann::json::parse(res->first);
  if(j["session_start_limit"]["remaining"] == 0){
    Log::error("no login remaining please attempt after " +
               std::to_string(j["session_start_limit"]["reset_after"].get<int>()) +
               " milliseconds");
    return;
  }
  api.defaultUrl = j["url"].get<std::string>().replace(0, 3, "https") + "/?v=10&encoding=json";
  handle.setUrl(api.defaultUrl);
  connect();
  getHeartbeatInterval();
  Log::dbg("interval is " + std::to_string(api.interval));
  sendIdent();
  listen();
}

NyaBot::~NyaBot(){
  stop = true;
  handle.wsClose(1000, "going away :3");
  Log::dbg("closed!"); 
}


void NyaBot::connect(){
  int timeToWait = 5;
  for(int attempts = 0; attempts < retryAmount; ++attempts, timeToWait *= 2){
    if(handle.perform() == OK){
      Log::dbg("connected to the websocket succesfully!");
      return;
    }
    else {
      if(timeToWait > 300) timeToWait = 300;
      Log::error("failed to connect to the gateway waiting " + std::to_string(timeToWait) + " seconds");
      std::this_thread::sleep_for(std::chrono::seconds(timeToWait));
    }
  }
}

void NyaBot::sendIdent(){
  Log::dbg("sending ident");
  // TODO: REWORK THIS IDENT PAYLOAD WHAT THE FUCK IS THIS
  std::string ident {R"({"op": 2, "d": {"token": ")" + api.token + R"(" , "intents": )" + std::to_string(api.intents) + R"(, "properties": {"os": "linux", "browser": "MeowLib", "device": "MeowLib"}}})"};
  if (handle.wsSend(ident, meowWs::meowWS_TEXT) > 0){
    Log::dbg("ident sent!");
  }
  else {
    Log::error("something went wrong");
  }
}


void NyaBot::getHeartbeatInterval(){
  std::string buf;
  // receive data from websocket
  try{
    meowWs::meowWsFrame frame;
    handle.wsRecv(buf, &frame);
    // initialize a json object with the data of buffer
    auto meowJson = nlohmann::json::parse(buf);
    // parse the data of heartbeat_interval into uint64_t and return it
    api.interval = meowJson["d"]["heartbeat_interval"];
  }
  catch(nlohmann::json::exception& e){
    Log::dbg("failed to parse buffer");
    Log::dbg("buffer is " + buf);
    std::exit(1);
  }

}

