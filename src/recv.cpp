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
