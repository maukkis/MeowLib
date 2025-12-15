#include "../../include/voice/voiceconnection.h"
#include "../../include/nyaBot.h"
#include <chrono>
#include <meowHttp/websocket.h>


void VoiceConnection::listen(){
  using namespace std::chrono_literals;
  auto sentHB = std::chrono::steady_clock::now();
  auto lastHB = std::chrono::steady_clock::now();
  while(!api.stop || bot->stop){
    std::string buf;
    if(std::chrono::steady_clock::now() - lastHB >= std::chrono::milliseconds(api.heartbeatInterval + 10000)){
      Log::warn("havent received a voice heartbeat in over 60 secs reconnecting");
      // we should still try resuming and we dont have to close the connection manually due to reconnect automatically calling it with 1012
      // we have to reset our times to avoid an edgecase causing us to always think we havent received a heartbeat
      lastHB = std::chrono::steady_clock::now();
      sentHB = std::chrono::steady_clock::now();
      continue;
    }
    if(std::chrono::steady_clock::now() - sentHB >= std::chrono::milliseconds(api.heartbeatInterval)){
      Log::dbg("sending voice heartbeat :3");
      nlohmann::json j;
      j["op"] = VoiceOpcodes::HEARTBEAT;
      if(api.seq != -1) j["d"]["seq_ack"] = api.seq;
      j["d"]["t"] = std::chrono::system_clock::now().time_since_epoch().count();
      Log::dbg(j.dump());
      if(handle.wsSend(j.dump(), meowWs::meowWS_TEXT) > 0){
        Log::dbg("sent voice heartbeat :3");
        sentHB = std::chrono::steady_clock::now();
      }
    }
    meowWs::meowWsFrame frame;
    size_t rlen = handle.wsRecv(buf, &frame);
    if (rlen < 1){
      continue;
    }
    if(frame.opcode == meowWs::meowWS_CLOSE){
      Log::warn("voice connection closed");
      uint16_t arf;
      std::memcpy(&arf, buf.data(), 2);
      arf = ntohs(arf);
      Log::dbg("code = " + std::to_string(arf) + "\nbuf = " + buf.substr(2));
      handle.wsClose(arf, buf.substr(2));
      return;
    }
    Log::dbg("voice received: " + std::to_string(frame.payloadLen) + " bytes");
    Log::dbg(buf);
    auto j = nlohmann::json::parse(buf);
    switch(j["op"].get<int>()){
      case VoiceOpcodes::READY:
        Log::dbg("voice ready");
        handleReady(j);
      break;
      case VoiceOpcodes::HEARTBEAT_ACK:
        lastHB = std::chrono::steady_clock::now();
        Log::dbg("got heartbeat ack");
      break;
    }
  }
}
