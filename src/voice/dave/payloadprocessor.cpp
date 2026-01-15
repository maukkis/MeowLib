#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h" 
#include <cstdint>
#include <cstring>
#include <meowHttp/websocket.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>

DaveProcessInfo Dave::processDavePayload(const std::string_view payload){
  DaveProcessInfo info;
  uint8_t opc{};
  std::string_view data;
  if(isBinaryEvent(opc)){
    opc = payload.at(2);
    data = payload.substr(3);
    std::memcpy(&info.seq, payload.data(), sizeof(info.seq));
    info.seq = ntohs(info.seq);
  } else {
    auto j = nlohmann::json::parse(payload);
    opc = j["op"];
    info.seq = j["d"]["seq_ack"];
  }
  Log::dbg(std::to_string(opc));
  auto fun = daveLut.at(opc - 21);
  if(!fun){
    Log::dbg("not implemented DAVE opcode!!!");
    return info;
  }
  info.toSend = fun(data);
  info.opcode = isBinaryEvent(opc) ? meowWs::meowWS_BINARY : meowWs::meowWS_TEXT;
  return info;
}
