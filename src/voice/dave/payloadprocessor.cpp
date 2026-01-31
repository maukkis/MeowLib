#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h"
#include "../../../include/helpers.h"
#include <cstdint>
#include <cstring>
#include <meowHttp/websocket.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>

DaveProcessInfo Dave::processDavePayload(const std::string_view payload, bool json){
  DaveProcessInfo info;
  uint8_t opc{};
  std::string_view data;
  if(!json){
    opc = payload.at(2);
    data = payload.substr(3);
    uint16_t arf = 0;
    std::memcpy(&arf, payload.data(), sizeof(arf));
    info.seq = ntohs(arf);
  } else {
    data = payload;
    auto j = nlohmann::json::parse(payload);
    opc = j["op"];
    jsonToOptional(info.seq, j["d"], "seq_ack");
  }
  Log::dbg(std::to_string(opc));
  auto fun = daveLut.at(opc - 21);
  if(!fun){
    Log::dbg("not implemented DAVE opcode!!!");
    return info;
  }
  info.toSend = fun(data);
  info.opcode = isBinaryEvent(opc) ? meowWs::meowWS_BINARY : meowWs::meowWS_TEXT;
  if((opc == VoiceOpcodes::DAVE_MLS_WELCOME || opc == VoiceOpcodes::DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION) && info.toSend){
    info.shouldSendKeyPackage = true;
  }
  return info;
}
