/* nyaBot simple discord bot written in C++ using libcurl
    Copyright (C) 2024  Luna
This file is part of nyaBot.

nyaBot is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

nyaBot is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with nyaBot; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "../meowHttp/src/includes/websocket.h"
#include <chrono>
#include <csignal>
#include <string>
#include <thread>
#include <unistd.h>
#include <iostream>
#include "includes/nyaBot.h"
#include <nlohmann/json.hpp>

NyaBot::NyaBot(){
  a = this;
  handle = meowWs::Websocket()
    .setUrl("https://gateway.discord.gg/?v=10&encoding=json");
  std::signal(SIGINT, NyaBot::signalHandler);
}

void NyaBot::run(const std::string& token){
  this->token = token;
  connect();
  getHeartbeatInterval();
  std::cout << "[*] interval is " << interval << '\n';
  sendIdent();
  std::thread heartbeatT{&NyaBot::sendHeartbeat, this};
  std::thread listenT{&NyaBot::listen, this};
  listenT.detach();
  heartbeatT.detach();
  while(!stop.load()){
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

NyaBot::~NyaBot(){
  stop.store(true);
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
  std::string ident {R"({"op": 2, "d": {"token": ")" + token + R"(" , "intents": 14, "properties": {"os": "linux", "browser": "meowLib", "device": "meowLib"}}})"};
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
      std::cerr << "[!] something went wrong\n";
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

