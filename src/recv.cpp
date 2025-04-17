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
#include <cstring>
#include <fstream>
#include <iostream>
#include "includes/nyaBot.h"
#include "includes/eventCodes.h"
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <thread>

void NyaBot::listen(){
  std::ofstream meowlog{"meow.log"}; 
  while (!stop.load()){
    std::string buf;
    meowWs::meowWsFrame frame;
    size_t rlen = handle.wsRecv(buf, &frame);
    if (rlen < 1){
      continue;
    }
    else {
      std::cout << "[*] received: " << rlen << " bytes\n";
      if(frame.opcode == meowWs::meowWS_CLOSE) {
        std::cout << "[!] connection closed\n";
        return;
      }
      try {
        auto meowJson = nlohmann::json::parse(buf);
        size_t op = meowJson["op"];
        switch(op){
          case HeartbeatACK:
            std::cout << "[*] server sent ACK\n";
          break;
          case Dispatch:
            std::string meow = meowJson["t"];
            if (meow == "READY"){
              sequence = meowJson["s"];
              meowJson = meowJson["d"];
              meowJson = meowJson["user"];
              appId = meowJson["id"];
              std::thread meow{onReadyF};
              meow.detach();
            }
            else if(meow == "GUILD_CREATE"){
              sequence = meowJson["s"];
              std::cout << "[*] got guild info!\n";
            }
            else if(meow == "INTERACTION_CREATE"){
              sequence = meowJson["s"];
              std::cout << "[*] got interaction\n";
              std::thread meowT{&NyaBot::interaction, this, meowJson};
              meowT.detach();
            }
            else if(meow == "MESSAGE_CREATE"){
              sequence = meowJson["s"];
              std::cout << "[*] new message got created\n";
            }
            else {
              std::cout << "[!] got unknown request printing to log\n";
              meowlog << buf << std::endl;
              sequence = meowJson["s"];
            }
          break;
        }
      }
      catch(nlohmann::json::exception& e){
        std::cout << "[!] got parse error :( partial data?\n";
      }
    }
  }
}
