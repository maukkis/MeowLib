#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <chrono>
#include <meowHttp/client.h>
#include <meowHttp/websocket.h>
#include <string>



void VoiceConnection::handleDave(const std::string_view buf, bool json){
  auto a = dave->processDavePayload(buf, json);
  api.seq = a.seq.value_or(api.seq);
  if(a.toSend){
    size_t size = handle.wsSend(*a.toSend, a.opcode);
    Log::dbg("sent " + std::to_string(size) + " bytes");
  }
}

void VoiceConnection::listen(){
  using namespace std::chrono_literals;
  auto sentHB = std::chrono::steady_clock::now();
  auto lastHB = std::chrono::steady_clock::now();
  while(!api.stop){
    try {
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
        switch(arf){
          case 4001:
          case 4002:
          case 4003:
          case 4004:
          case 4005:
          case 4016:
            reconnect();
          break;
          case 4006:
          case 4021:
            api.stop = true;
            fcv.notify_all();
            return;
          break;
          case 4014:
          case 4022:
            reconnect(false, true);
          break;
          default:
            reconnect(true);
          break;
        }
        continue;
      }
      Log::dbg("voice received: " + std::to_string(frame.payloadLen) + " bytes");
      if(frame.opcode == meowWs::meowWS_BINARY){
        handleDave(buf);
        continue;
      }
      auto j = nlohmann::json::parse(buf);
      if(isDaveEvent(j["op"])){
        handleDave(buf, true);
        continue;
      }
      switch(j["op"].get<int>()){
        case VoiceOpcodes::READY:
          Log::dbg("voice ready");
          handleReady(j);
        break;
        case VoiceOpcodes::SESSION_DESCRIPTION:
          handleSessionDescription(j);
        break;
        case VoiceOpcodes::HEARTBEAT_ACK:
          lastHB = std::chrono::steady_clock::now();
          Log::dbg("got heartbeat ack");
        break;
        case VoiceOpcodes::SPEAKING:
        break;
        case VoiceOpcodes::CLIENT_CONNECT:
          dave->addUsers(j["d"]["user_ids"]);
        break;
        case VoiceOpcodes::CLIENT_DISCONNECT:
          dave->removeUsers({j["d"]["user_id"]});
        break;
        case 15: // undocumented
        case 12:
        case 18:
        case 20:
        break;
        default:
          Log::dbg("got unknown voice payload");
          Log::dbg(j.dump());
        break;
      }
    } catch(meowHttp::Exception& e){
        Log::dbg(e.what());
        reconnect(true);
        lastHB = std::chrono::steady_clock::now();
        sentHB = std::chrono::steady_clock::now();
        continue;
    }
  }
  fcv.notify_all();
}
