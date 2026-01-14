#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h" 
#include <cstring>
#include <netinet/in.h>

DaveProcessInfo Dave::processDavePayload(const std::string_view payload){
  DaveProcessInfo info;
  std::memcpy(&info.seq, payload.data(), sizeof(info.seq));
  info.seq = ntohs(info.seq);
  auto fun = daveLut.at(payload.at(2) - 21);
  if(!fun){
    Log::dbg("not implemented DAVE opcode!!!");
    return info;
  }
  info.toSend = fun(payload.substr(3));
  return info;
}
