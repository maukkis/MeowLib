#include "../../../include/voice/dave/dave.h"

void Dave::addToLut(std::function<std::optional<std::string>(const std::string_view)> f, VoiceOpcodes opc){
  daveLut.at(opc - 21) = f;
}

bool isBinaryEvent(int opcode){
  switch(opcode){
    case VoiceOpcodes::DAVE_MLS_External_Sender:
    case VoiceOpcodes::DAVE_MLS_Key_Package:
    case VoiceOpcodes::DAVE_MLS_Proposals:
    case VoiceOpcodes::DAVE_MLS_Commit_Welcome:
    case VoiceOpcodes::DAVE_MLS_Welcome:
    return true;
  }
  return false;
}

bool isDaveEvent(int opcode){
  if(opcode >= 21) return true;
  return false;
}
