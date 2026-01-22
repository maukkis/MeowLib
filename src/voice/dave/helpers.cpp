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

void Dave::addUsers(const std::vector<std::string>& a){
  for(const auto& user : a){
    users.emplace(std::stoull(user));
  }
}


void Dave::removeUsers(const std::vector<std::string>& a){
  for(const auto& user : a){
    users.erase(std::stoull(user));
  }
}


uint64_t snowflakeFromCredential(const mls::bytes_ns::bytes& val){
  uint64_t a{};
  for(const uint64_t byte : val){
    a = (a << 8) | byte;
  }
  return a;
}


void Dave::createEncryptor(){
  if(!currentState){
    Log::error("cannot create an encryptor without a state");
    return;
  }
  auto userId = bytesFrom(std::stoull(botId), sizeof(uint64_t));
  auto baseSecret = currentState->do_export(std::string(exporterLabel), userId, 16);
  keyratchet = mls::HashRatchet(getCipherSuite(), baseSecret);
  auto key = getKeyForGeneration(0); // when we create an encryptor for a new epoch we start the generation from zero always
  if(!encryptor)
    encryptor.emplace(key, this);
  else {
    encryptor->nonce = 0;
    encryptor->previousNonce = 0;
    encryptor->generation = 0;
    encryptor->changeKey(key);
  }
}

std::vector<uint8_t> Dave::getKeyForGeneration(uint32_t generation){
  auto [key, nonce] = keyratchet.get(generation);
  return key;
}

bool isBinaryEvent(int opcode){
  switch(opcode){
    case VoiceOpcodes::DAVE_MLS_EXTERNAL_SENDER:
    case VoiceOpcodes::DAVE_MLS_KEY_PACKAGE:
    case VoiceOpcodes::DAVE_MLS_PROPOSALS:
    case VoiceOpcodes::DAVE_MLS_COMMIT_WELCOME:
    return true;
  }
  return false;
}

bool isDaveEvent(int opcode){
  if(opcode >= 21) return true;
  return false;
}
