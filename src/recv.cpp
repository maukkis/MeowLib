#include "../meowHttp/src/includes/websocket.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include "../include/nyaBot.h"
#include "../include/eventCodes.h"
#include <netinet/in.h>
#include <stdio.h>
#include <nlohmann/json.hpp>


void NyaBot::listen(){
  using namespace std::chrono_literals;
  std::ofstream meowlog{"meow.log"};
  auto sentHB = std::chrono::steady_clock::now();
  auto lastHB = std::chrono::steady_clock::now();
  srand(std::time(0));
  float random = ((float) rand()) / (float) RAND_MAX;
  std::cout << "starting to heartbeat with jitter of: " << random << std::endl;
  while (!stop){
    std::string buf;
    try {
      if(std::chrono::steady_clock::now() - lastHB >= 60s){
        std::cout << "[!] havent received heartbeat in over 60 secs reconnecting" << std::endl;
        handle.wsClose();
        reconnect(false);
        // we have to reset our times to avoid an edgecase causing us to always think we havent received a heartbeat
        lastHB = std::chrono::steady_clock::now();
        sentHB = std::chrono::steady_clock::now();
        continue;
      }
      if(std::chrono::steady_clock::now() - sentHB >= std::chrono::milliseconds(static_cast<int>(api.interval*random))){
        std::cout << "sending heartbeat :3\n";
        nlohmann::json j;
        j["op"] = Heartbeat;
        j["d"] = api.sequence;
        if(handle.wsSend(j.dump(), meowWs::meowWS_TEXT) > 0){
          std::cout << "sent heartbeat :3\n";
          sentHB = std::chrono::steady_clock::now();
        }
        random = 1;
      }
      meowWs::meowWsFrame frame;
      size_t rlen = handle.wsRecv(buf, &frame);
      if (rlen < 1){
        continue;
      }
      std::cout << "[*] received: " << rlen << " bytes\n";
      if(frame.opcode == meowWs::meowWS_CLOSE) {
        std::cout << "[!] connection closed\n";
        uint16_t arf;
        memcpy(&arf, buf.data(), 2);
        arf = ntohs(arf);
        std::cout << "code = " <<  arf << "\nbuf = " << buf.substr(2) << std::endl;
        handle.wsClose(arf, buf.substr(2));
        switch(arf){
          case 1000:
          case 1001:
          case 4000:
          case 4001:
          case 4002:
          case 4003:
          case 4005:
          case 4008:
          case 4009:
            reconnect(true);
          break;
          case 4007:
          case 1005:
          case 1006:
          case 1011:
            reconnect(false);
          break;
          default:
            stop = true;
            std::cout << "[!] something went horribly wrong" << std::endl;
            return;
        }
        continue;
      }
      auto meowJson = nlohmann::json::parse(buf);
      size_t op = meowJson["op"];
      switch(op){
        case HeartbeatACK:
          std::cout << "[*] server sent ACK\n";
          lastHB = std::chrono::steady_clock::now();
        break;
        case Reconnect:
          std::cout << "got reconnectTwT\nwaiting for close\n";
        break; 
        case Dispatch:
          {
            std::string meow = meowJson["t"];
            api.sequence = meowJson["s"];
            if(dispatchHandlers.contains(meow)){
              std::thread{dispatchHandlers[meow], meowJson}.detach();
            }
            else {
              std::cout << "unknown dispatch printing to log" << std::endl;
              meowlog << buf << std::endl;
            }
          }
        break;
        case InvalidSession:
          std::cout << "invalid session owo reconnecting without resuming :3\n";
          reconnect(false);
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
    catch(meowHttp::Exception &e){
      std::cout << e.what() << std::endl;
      if(e.closed()) reconnect(true);
      else {
        std::cout << "fatal error exiting :3\n";
        return;
      }
    }
  }
}

