#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h"
#include <cstdint>
#include <string>
#include <vector>


constexpr std::string_view exporterLabel = "Discord Secure Frames v0";

mls::CipherSuite getCipherSuite(){
  return mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256;
}


void Dave::addToLut(std::function<std::optional<std::string>(const std::string_view)> f, VoiceOpcodes opc){
  daveLut.at(opc - 21) = f;
}



void Dave::createEncryptor(){
  if(!currentState){
    Log::error("cannot create an encryptor without a state");
    return;
  }
  auto userId = bytesFrom(std::stoull(botId), sizeof(uint64_t));
  auto baseSecret = currentState->do_export(std::string(exporterLabel), userId, 16);
  keyratchet = mls::HashRatchet(getCipherSuite(), baseSecret);
  auto key = getKeyForGeneration(0);
  encryptor = Encryptor(key, this);
}

std::vector<uint8_t> Dave::getKeyForGeneration(uint32_t generation){
  auto [key, nonce] = keyratchet.get(generation);
  return key;
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
