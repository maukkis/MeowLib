#include "../meowHttp/src/includes/websocket.h"
#include <csignal>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include "../include/eventCodes.h"
#include "../include/nyaBot.h"
#include <nlohmann/json.hpp>

NyaBot::NyaBot(){
  a = this;
  handle = meowWs::Websocket()
    .setUrl("https://gateway.discord.gg/?v=10&encoding=json");
  std::signal(SIGINT, NyaBot::signalHandler);
}

meow NyaBot::reconnect(bool resume){
  if(handle.wsClose(1012, "arf") != OK){
    Log::Log("woof?"); 
  }
  if(api.resumeUrl.find("wss") != std::string::npos) api.resumeUrl.replace(0, 3, "https");
  handle.setUrl(api.resumeUrl);
  connect();
  getHeartbeatInterval();
  if(resume){
    nlohmann::json j;
    j["op"] = Resume;
    j["d"]["token"] = api.token;
    j["d"]["session_id"] = api.sesId;
    j["d"]["seq"] = api.sequence;
    if(handle.wsSend(j.dump(), meowWs::meowWS_TEXT) > 0 ){
      Log::Log("sent resume!");
    }
  } else {
    sendIdent();
  }
  return OK;
}

void NyaBot::run(const std::string_view token){
  this->api.token = token;
  connect();
  getHeartbeatInterval();
  Log::Log("interval is " + std::to_string(api.interval));
  sendIdent();
  listen();
}

NyaBot::~NyaBot(){
  stop = true;
  handle.wsClose(1000, "going away :3");
  Log::Log("closed!"); 
}


void NyaBot::connect(){
  if(handle.perform() == OK){
    Log::Log("connected to the websocket succesfully!");
  }
  else {
    Log::Log("something went wrong");
    std::exit(1);
  }
  
}

void NyaBot::sendIdent(){
  Log::Log("sending ident");
  std::string ident {R"({"op": 2, "d": {"token": ")" + api.token + R"(" , "intents": 16, "properties": {"os": "linux", "browser": "meowLib", "device": "meowLib"}}})"};
  if (handle.wsSend(ident, meowWs::meowWS_TEXT) > 0){
    Log::Log("ident sent!");
  }
  else {
    Log::Log("something went wrong");
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
    Log::Log("failed to parse buffer");
    Log::Log("buffer is " + buf);
    std::exit(1);
  }

}

