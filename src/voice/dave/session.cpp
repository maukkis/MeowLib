#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h" 
#include "hpke/random.h"
#include <mlspp/mls/messages.h>
#include <mlspp/tls/tls_syntax.h>
#include <mlspp/bytes/bytes.h>
#include <cstdint>
#include <mlspp/mls/credential.h>
#include <mlspp/mls/crypto.h>
#include <mlspp/mls/core_types.h>

namespace {
mls::Capabilities getDefaultDaveMLSCapabilities(){
  auto cap = mls::Capabilities::create_default();
  cap.cipher_suites = {mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256};
  return cap;
}

template<typename T>
mls::bytes_ns::bytes genBEBytes(T&& value, size_t size){
  auto buf = mls::bytes_ns::bytes();
  buf.reserve(size);
  for(size_t i = size - 1; i >= 0; --i){
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
  // we wont have an external sender at this point so we cannot create a group
}

std::optional<std::string> Dave::processExternalSender(const std::string_view payload){
  externalSender = mls::tls::get<mls::ExternalSender>(std::vector<uint8_t>(payload.begin(), payload.end()));
  if(!leaf) [[unlikely]]{
    Log::dbg("?????");
    return std::nullopt;
  }
  // we can now create our pending group
  pendingState = mls::State(
    groupId,
    mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256,
    hpekKey,
    sigPrivKey,
    *leaf,
    generateStateExtensionList(*externalSender)
  );
  return std::nullopt;
}

std::optional<std::string> Dave::processProposals(const std::string_view s){
  if(s.at(0) == 1){
    todo("proposal revokes not implemented");
  }
  Log::dbg("processing proposals");
  mls::tls::istream stream(std::vector<uint8_t>(s.substr(1).begin(), s.substr(1).end()));
  std::vector<mls::MLSMessage> messages;
  stream >> messages;
  for(const auto& a : messages){
    pendingState->handle(a);
  }
  auto secret = mls::hpke::random_bytes(mls::CipherSuite(mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256).secret_size());
  auto [commit, welcome, state] = pendingState->commit(secret, std::nullopt, {});
  cachedState = state; // seemingly we need this state always idk maybe im missing something from the docs but libdave does it like this maybe DAVE 1.0 thing?
  mls::tls::ostream ostream;
  ostream << commit << welcome;
  return std::string(ostream.bytes().begin(), ostream.bytes().end());
}

void Dave::initLeaf(const std::string& userId) {
  sigPrivKey = mls::SignaturePrivateKey::generate(mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256);
  hpekKey = mls::HPKEPrivateKey::generate(mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256);
  leaf = mls::LeafNode(
    mls::CipherSuite::ID::P256_AES128GCM_SHA256_P256,
    hpekKey.public_key,
    sigPrivKey.public_key,
    generateDaveMLSCredential(userId),
    getDefaultDaveMLSCapabilities(),
    mls::Lifetime::create_default(),
    mls::ExtensionList{},
    sigPrivKey
  );
}
