#include "../meowHttp/src/includes/websocket.h"
#include <chrono>
#include <csignal>
#include <pthread.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <iostream>
#include "includes/eventCodes.h"
#include "includes/nyaBot.h"
#include <nlohmann/json.hpp>

NyaBot::NyaBot(){
  a = this;
  handle = meowWs::Websocket()
    .setUrl("https://gateway.discord.gg/?v=10&encoding=json");
  std::signal(SIGINT, NyaBot::signalHandler);
}

meow NyaBot::reconnect(const std::string& sesId, std::string& reconnectUrl, bool resume){
  reconnecting.store(true);
  pthread_cancel(hbT.native_handle());
  hbT.detach();
  if(handle.wsClose(1000, "arf") != OK){
    std::cout << "woof?\n"; 
  }
  reconnectUrl.replace(0, 3, "https");
  handle.setUrl(reconnectUrl);
  connect();
  getHeartbeatInterval();
  nlohmann::json j;
  if(resume){
    j["op"] = Resume;
    j["d"]["token"] = token;
    j["d"]["session_id"] = sesId;
    j["d"]["seq"] = sequence;
    if(handle.wsSend(j.dump(), meowWs::meowWS_TEXT) > 0 ){
      std::cout << "sent resume!\n";
    }
  } else {
    sendIdent();
  }
  hbT = std::move(std::thread{&NyaBot::sendHeartbeat, this});
  reconnecting.store(false);
  return OK;
}

void NyaBot::run(const std::string& token){
  this->token = token;
  connect();
  getHeartbeatInterval();
  std::cout << "[*] interval is " << interval << '\n';
  sendIdent();
  hbT = std::move(std::thread{&NyaBot::sendHeartbeat, this});
  std::thread listenT{&NyaBot::listen, this};
  listenT.detach();
  while(!stop.load()){
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

NyaBot::~NyaBot(){
  stop.store(true);
  pthread_cancel(hbT.native_handle());
  hbT.detach();
  handle.wsClose(1000, "going away :3");
  std::cout << "[*] closed!\n"; 
}


void NyaBot::connect(){
  if(handle.perform() == OK){
    std::cout << "[*] connected to the websocket succesfully!\n";
  }
  else {
    std::cerr << "[!] something went wrong\n";
    std::exit(1);
  }
  
}

void NyaBot::sendIdent(){
  std::cout << "[*] sending ident\n";
  std::string ident {R"({"op": 2, "d": {"token": ")" + token + R"(" , "intents": 16, "properties": {"os": "linux", "browser": "meowLib", "device": "meowLib"}}})"};
  if (handle.wsSend(ident, meowWs::meowWS_TEXT) > 0){
    std::cout << "[*] ident sent!\n";
  }
  else {
    std::cout << "[!] something went wrong\n";
  }
}


void NyaBot::sendHeartbeat(){
  srand(std::time(0));
  float random = ((float) rand()) / (float) RAND_MAX;
  std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(interval * random)));
  while (!stop.load()) {
    const std::string heartbeat{R"({"op": 1,"d": )" + std::to_string(sequence) + R"(})"};
    if (handle.wsSend(heartbeat, meowWs::meowWS_TEXT) > 0){
      std::cout << "[*] hearbeat sent succesfully!\n";
    }
    else{
      std::cerr << "[!] HBT something went wrong\n";
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
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
    interval = meowJson["d"]["heartbeat_interval"];
  }
  catch(nlohmann::json::exception& e){
    std::cout << "[!] failed to parse buffer\n";
    std::cout << "[!] buffer is " << buf << '\n';
    std::exit(1);
  }

}

