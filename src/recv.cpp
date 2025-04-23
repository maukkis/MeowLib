#include "../meowHttp/src/includes/websocket.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include "includes/nyaBot.h"
#include "includes/eventCodes.h"
#include <netinet/in.h>
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <thread>

void NyaBot::listen(){
  std::ofstream meowlog{"meow.log"};
  std::string sesId;
  std::string resumeUrl;
  std::time_t lastHB = std::time(nullptr);
  while (!stop.load()){
    std::string buf;
    if(std::time(nullptr) - lastHB >= 60){
      std::cout << "[!] havent received heartbeat in over 60 secs reconnecting" << std::endl;
      reconnect(sesId, resumeUrl, false);
    }
    meowWs::meowWsFrame frame;
    handleLock.lock();
    size_t rlen = handle.wsRecv(buf, &frame);
    handleLock.unlock();
    if (rlen < 1){
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }
    else {
      std::cout << "[*] received: " << rlen << " bytes\n";
      if(frame.opcode == meowWs::meowWS_CLOSE) {
        std::cout << "[!] connection closed\n";
        uint16_t arf;
        memcpy(&arf, buf.data(), 2);
        arf = ntohs(arf);
        std::cout << "code = " <<  arf << "\nbuf = " << buf.substr(2) << std::endl;
          handleLock.lock();
          handle.wsClose(arf, buf.substr(2));
          handleLock.unlock();
        switch(arf){
          case 4000:
          case 4001:
          case 4002:
          case 4003:
          case 4005:
          case 4008:
          case 4009:
            reconnect(sesId, resumeUrl, true);
          break;
          case 4007:
          case 1000:
          case 1001:
          case 1005:
          case 1006:
          case 1011:
            reconnect(sesId, resumeUrl, false);
          break;
          default:
            stop.store(true);
            std::cout << "[!] something went horribly wrong" << std::endl;
            return;
        }
      }
      try {
        auto meowJson = nlohmann::json::parse(buf);
        size_t op = meowJson["op"];
        switch(op){
          case HeartbeatACK:
            std::cout << "[*] server sent ACK\n";
            lastHB = std::time(nullptr);
          break;
          case Reconnect:
            std::cout << "got reconnectTwT\n";
            reconnect(sesId, resumeUrl, true);
          break; 
          case Dispatch:
            {
              std::string meow = meowJson["t"];
              if (meow == "READY"){
                sequence = meowJson["s"];
                meowJson = meowJson["d"];
                sesId = meowJson["session_id"];
                resumeUrl = meowJson["resume_gateway_url"];
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
            }
          break;
          default:
            std::cout << "barks?\n";
            meowlog << buf << std::endl;
          break;
        }
      }
      catch(nlohmann::json::exception& e){
        std::cout << "[!] got parse error :( partial data?\n";
        meowlog << buf << std::endl;
      }
    }
  }
}
