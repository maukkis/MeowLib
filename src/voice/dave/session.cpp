#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h" 
#include <mlspp/hpke/random.h>
#include <mlspp/mls/messages.h>
#include <mlspp/tls/tls_syntax.h>
#include <mlspp/bytes/bytes.h>
#include <cstdint>
#include <mlspp/mls/credential.h>
#include <mlspp/mls/crypto.h>
#include <mlspp/mls/core_types.h>

namespace {
mls::CipherSuite getCipherSuite(){
  return mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256;
}


mls::Capabilities getDefaultDaveMLSCapabilities(){
  auto cap = mls::Capabilities::create_default();
  cap.cipher_suites = {mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256};
  return cap;
}

template<typename T>
mls::bytes_ns::bytes genBEBytes(T&& value, size_t size){
  auto buf = mls::bytes_ns::bytes();
  buf.reserve(size);
  for(ssize_t i = size - 1; i >= 0; --i){
    buf.push_back(static_cast<uint8_t>(std::forward<T>(value) >> (i * 8)));
  }
  return buf;
} 

mls::ExtensionList generateStateExtensionList(const mls::ExternalSender& a){
  mls::ExtensionList list;
  list.add(mls::ExternalSendersExtension{{{a.signature_key, a.credential}}});
  return list;
}

mls::Credential generateDaveMLSCredential(const std::string& userId){
  uint64_t snowflake = std::stoull(userId);
  return mls::Credential::basic(genBEBytes(snowflake, sizeof(snowflake)));
}



}


Dave::Dave(const std::string& userId, int groupId){
  initLeaf(userId);
  this->groupId = genBEBytes(groupId, sizeof(groupId));
  addToLut(std::bind(&Dave::processExternalSender, this, std::placeholders::_1), VoiceOpcodes::DAVE_MLS_External_Sender);
  addToLut(std::bind(&Dave::processProposals, this, std::placeholders::_1), VoiceOpcodes::DAVE_MLS_Proposals);
  // we wont have an external sender at this point so we cannot create a group
}


std::string Dave::getKeyPackagePayload(){
  joinInitKey = mls::HPKEPrivateKey::generate(getCipherSuite());
  keyPackage = mls::KeyPackage(
    getCipherSuite(),
    joinInitKey.public_key,
    leaf.value(),
    mls::ExtensionList{},
    sigPrivKey
  );
  auto a = mls::tls::marshal(keyPackage);
  std::string str(a.begin(), a.end());
  return static_cast<char>(VoiceOpcodes::DAVE_MLS_Key_Package) + str;
}



std::optional<std::string> Dave::processExternalSender(const std::string_view payload){
  Log::dbg("processing external sender");
  externalSender = mls::tls::get<mls::ExternalSender>(std::vector<uint8_t>(payload.begin(), payload.end()));
  if(!leaf){
    Log::dbg("???");
  }
  // we can now create our pending group
  pendingState = mls::State(
    groupId,
    getCipherSuite(),
    hpekKey,
    sigPrivKey,
    leaf.value(),
    generateStateExtensionList(externalSender.value())
  );
  return std::nullopt;
}

std::optional<std::string> Dave::processProposals(const std::string_view s){
  if(s.at(0) == 1){
    todo("proposal revokes not implemented");
  }
  Log::dbg("processing proposals");
  mls::tls::istream stream(std::vector<uint8_t>(s.begin(), s.end()));
  std::vector<mls::MLSMessage> messages;
  stream >> messages;
  for(const auto& a : messages){
    pendingState->handle(a);
  }
  auto secret = mls::hpke::random_bytes(getCipherSuite().secret_size());
  auto [commit, welcome, state] = pendingState->commit(secret, std::nullopt, {});
  cachedState = state; // seemingly we need this state always idk maybe im missing something from the docs but libdave does it like this maybe DAVE 1.0 thing?
  mls::tls::ostream ostream;
  ostream << commit << welcome;
  return std::string(ostream.bytes().begin(), ostream.bytes().end());
}

void Dave::initLeaf(const std::string& userId) {
  sigPrivKey = mls::SignaturePrivateKey::generate(getCipherSuite());
  hpekKey = mls::HPKEPrivateKey::generate(getCipherSuite());
  leaf = mls::LeafNode(
    getCipherSuite(),
    hpekKey.public_key,
    sigPrivKey.public_key,
    generateDaveMLSCredential(userId),
    getDefaultDaveMLSCapabilities(),
    mls::Lifetime::create_default(),
    mls::ExtensionList{},
    sigPrivKey
  );
  if(leaf.has_value()) Log::dbg("stupid");
}
