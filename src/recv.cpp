#include "../meowHttp/src/includes/websocket.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include "../include/nyaBot.h"
#include "../include/eventCodes.h"
#include "../include/queue.h"
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>


void NyaBot::listen(){
  using namespace std::chrono_literals;
  std::ofstream meowlog{"meow.log"};
  auto sentHB = std::chrono::steady_clock::now();
  auto lastHB = std::chrono::steady_clock::now();
  srand(std::time(0));
  float random = ((float) rand()) / (float) RAND_MAX;
  Log::Log("starting to heartbeat with jitter of: " + std::to_string(random)); 
  while (!stop){
    std::string buf;
    try {
      if(std::chrono::steady_clock::now() - lastHB >= 60s){
        Log::Log("havent received heartbeat in over 60 secs reconnecting");
        // we should still try resuming and we dont have to close the connection manually due to reconnect automatically calling it with 1012
        reconnect(true);
        // we have to reset our times to avoid an edgecase causing us to always think we havent received a heartbeat
        lastHB = std::chrono::steady_clock::now();
        sentHB = std::chrono::steady_clock::now();
        continue;
      }
      if(std::chrono::steady_clock::now() - sentHB >= std::chrono::milliseconds(static_cast<int>(api.interval*random))){
        Log::Log("sending heartbeat :3");
        nlohmann::json j;
        j["op"] = Heartbeat;
        j["d"] = api.sequence;
        if(handle.wsSend(j.dump(), meowWs::meowWS_TEXT) > 0){
          Log::Log("sent heartbeat :3");
          sentHB = std::chrono::steady_clock::now();
        }
        random = 1;
      }
      while(!queue.empty()) handle.wsSend(queue.pop(), meowWs::meowWS_TEXT);
      meowWs::meowWsFrame frame;
      size_t rlen = handle.wsRecv(buf, &frame);
      if (rlen < 1){
        continue;
      }
      Log::Log("received: " + std::to_string(frame.payloadLen) + " bytes");
      if(frame.opcode == meowWs::meowWS_CLOSE) {
        Log::Log("connection closed");
        uint16_t arf;
        memcpy(&arf, buf.data(), 2);
        arf = ntohs(arf);
        Log::Log("code = " + std::to_string(arf) + "\nbuf = " + buf.substr(2));
        handle.wsClose(arf, buf.substr(2));
        switch(arf){
          case 4000:
          case 4001:
          case 4002:
          case 4003:
          case 4005:
          case 4008:
          case 4009:
            reconnect(true);
          break;
          case 1000:
          case 1001:
          case 4007:
          case 1005:
          case 1006:
          case 1011:
            reconnect(false);
          break;
          default:
            stop = true;
            Log::Log("something went horribly wrong");
            return;
        }
        continue;
      }
      auto meowJson = nlohmann::json::parse(buf);
      size_t op = meowJson["op"];
      switch(op){
        case HeartbeatACK:
          Log::Log("server sent ACK");
          lastHB = std::chrono::steady_clock::now();
        break;
        case Reconnect:
          Log::Log("got reconnectTwT closing and resuming");
          reconnect(true);
        break; 
        case Dispatch:
          {
            std::string meow = meowJson["t"];
            api.sequence = meowJson["s"];
            if(dispatchHandlers.contains(meow)){
              std::thread{dispatchHandlers[meow], meowJson}.detach();
            }
            else {
              Log::Log("unknown dispatch printing to log");
              meowlog << buf << std::endl;
            }
          }
        break;
        case InvalidSession:
          Log::Log("invalid session owo reconnecting without resuming :3");
          reconnect(false);
        break;
        default:
          Log::Log("barks?");
          meowlog << buf << std::endl;
        break;
      }
    }
    catch(nlohmann::json::exception& e){
      Log::Log("got parse error :( partial data?");
      meowlog << buf << std::endl;
    }
    catch(meowHttp::Exception &e){
      Log::Log(e.what());
      if(e.closed()) reconnect(true);
      else {
        Log::Log("fatal error exiting :3");
        return;
      }
    }
  }
}

