#include "../../../include/voice/dave/dave.h"

void Dave::addToLut(std::function<std::optional<std::string>(const std::string_view)> f, VoiceOpcodes opc){
  daveLut.at(opc - 21) = f;
}
