#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h" 
#include <mlspp/mls/state.h>
#include <exception>
#include <mlspp/mls/key_schedule.h>
#include <nlohmann/json.hpp>
#include <mlspp/hpke/random.h>
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
  cap.credentials = {mls::CredentialType::basic};
  return cap;
}


template<typename T>
mls::bytes_ns::bytes genBEBytes(const T& value, size_t size){
  auto buf = mls::bytes_ns::bytes();
  buf.reserve(size);
  for(ssize_t i = size - 1; i >= 0; --i){
    buf.push_back(static_cast<uint8_t>(value >> (i * 8)));
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
  auto a = genBEBytes(snowflake, sizeof(snowflake));
  return mls::Credential::basic(a);
}

}


Dave::Dave(const std::string& userId, uint64_t groupId){
  initLeaf(userId);
  botId = userId;
  this->groupId = genBEBytes(groupId, sizeof(groupId));
  addToLut(std::bind(&Dave::processExternalSender, this, std::placeholders::_1), VoiceOpcodes::DAVE_MLS_EXTERNAL_SENDER);
  addToLut(std::bind(&Dave::processProposals, this, std::placeholders::_1), VoiceOpcodes::DAVE_MLS_PROPOSALS);
  addToLut(std::bind(&Dave::processCommitTransition, this ,std::placeholders::_1), VoiceOpcodes::DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION);
  addToLut(std::bind(&Dave::executeTransition, this, std::placeholders::_1), VoiceOpcodes::DAVE_EXECUTE_TRANSITION);
  addToLut(std::bind(&Dave::processWelcome, this, std::placeholders::_1), VoiceOpcodes::DAVE_MLS_WELCOME);
  // we wont have an external sender at this point so we cannot create a group
}

bool Dave::ready(){
  return currentState.has_value() && encryptor.has_value();
}


std::string Dave::getKeyPackagePayload(){
  // we should ALWAYS generate a new key package
  // ref: rfc 9420 sect: 16.8
  joinInitKey = std::make_unique<mls::HPKEPrivateKey>(mls::HPKEPrivateKey::generate(getCipherSuite()));
  keyPackage = std::make_unique<mls::KeyPackage>(
    getCipherSuite(),
    joinInitKey->public_key,
    *leaf,
    mls::ExtensionList{},
    *sigPrivKey
  );
  auto a = mls::tls::marshal(*keyPackage);
  a.insert(a.begin(), VoiceOpcodes::DAVE_MLS_KEY_PACKAGE);
  std::string str(a.begin(), a.end());
  uint8_t b = str.at(0);
  Log::dbg(std::to_string(b));
  return str;
}



std::optional<std::string> Dave::processExternalSender(const std::string_view payload){
  Log::dbg("processing external sender");
  externalSender = mls::tls::get<mls::ExternalSender>(std::vector<uint8_t>(payload.begin(), payload.end()));
  if(!leaf){
    Log::dbg("leaf is somehow null wtf");
    return std::nullopt;
  }
  // we can now create our pending group
  pendingState = mls::State(
    groupId,
    getCipherSuite(),
    *hpekKey,
    *sigPrivKey,
    *leaf,
    generateStateExtensionList(externalSender.value())
  );
  return std::nullopt;
}

std::optional<std::string> Dave::processProposals(const std::string_view s){
  if(s.at(0) == 1){
    todo("proposal revokes not implemented");
  }
  commitState = (pendingState ? *pendingState : *currentState);
  Log::dbg("processing proposals");
  try{
    mls::tls::istream stream(std::vector<uint8_t>(s.begin(), s.end()));
    bool meow;
    stream >> meow;
    std::vector<mls::MLSMessage> messages;
    stream >> messages;
    for(const auto& a : messages){
      Log::dbg("handling message");
      auto msg = commitState->unwrap(a);
      if(!isValidProposal(msg)){
        Log::error("not a valid proposal bailing out from handling proposals");
        return std::nullopt;
      }
      commitState->handle(msg);
    }
    auto secret = mls::hpke::random_bytes(commitState->cipher_suite().secret_size());
    auto co = mls::CommitOpts{
      {},
      true,
      false,
      {}
    };
    auto [commit, welcome, state] = commitState->commit(secret, co, {});
    cachedState = state; // seemingly we need this state always idk maybe im missing something from the docs but libdave does it like this maybe DAVE 1.0 thing?
    mls::tls::ostream ostream;
    ostream << commit;
    Log::dbg(std::format("{} welcome messages", welcome.secrets.size()));
    if(welcome.secrets.size() > 0){
      ostream << welcome;
    }
    auto bytes = ostream.bytes();
    bytes.insert(bytes.begin(), VoiceOpcodes::DAVE_MLS_COMMIT_WELCOME);
    std::string data(bytes.begin(), bytes.end());
    return data;
  } catch(std::exception& e){
    Log::dbg(std::string("got exception ") + e.what());
    return std::nullopt;
  }
}


std::optional<std::string> Dave::processCommitTransition(const std::string_view a){
  mls::tls::istream istream(std::vector<uint8_t>(a.begin(), a.end()));
  uint16_t transitionId{};
  istream >> transitionId;
  Log::dbg(std::format("handling commit transition with an id of: {}", transitionId));
  mls::MLSMessage message;
  istream >> message;
  auto state = commitState->handle(message, *cachedState);
  currentState = std::move(state);
  Log::dbg(std::format("succesfully moved to a new epoch our leaf index is: {} and the epoch is: {}", currentState->index().val, currentState->epoch()));
  cachedState.reset();
  commitState.reset();
  pendingState.reset();
  createEncryptor();
  if(transitionId != 0){
    nlohmann::json j;
    j["op"] = VoiceOpcodes::DAVE_TRANSITION_READY;
    j["d"]["transition_id"] = transitionId;
    return j.dump();
  }
  return std::nullopt;
}


std::optional<std::string> Dave::processWelcome(const std::string_view a){
  mls::tls::istream istream(std::vector<uint8_t>(a.begin(), a.end()));
  uint16_t transitionId{};
  istream >> transitionId;
  Log::dbg(std::format("got a welcome for transition id: {}", transitionId));
  mls::Welcome welcome;
  istream >> welcome;

  currentState = mls::State(
    *joinInitKey,
    *hpekKey,
    *sigPrivKey,
    *keyPackage,
    welcome,
    std::nullopt,
    {}
  );
  if(!isValidWelcomeState()){
    currentState.reset();
    Log::error("failed to process welcome");
    nlohmann::json j;
    j["op"] = VoiceOpcodes::DAVE_MLS_INVALID_COMMIT_WELCOME;
    j["d"]["transition_id"] = transitionId;
    return j.dump();
  }
  Log::dbg(std::format("succesfully welcomed into the group our leaf index is: {} and the epoch is: {}", currentState->index().val, currentState->epoch()));
  cachedState.reset();
  commitState.reset();
  pendingState.reset();
  createEncryptor();
  return std::nullopt;
}


std::optional<std::string> Dave::executeTransition(const std::string_view a){
  auto j = nlohmann::json::parse(a);
  Log::dbg(std::format("executing transition with an id of {}", j["d"]["transition_id"].get<int>()));
  return std::nullopt;
}

void Dave::initLeaf(const std::string& userId) {
  sigPrivKey = std::make_unique<mls::SignaturePrivateKey>(mls::SignaturePrivateKey::generate(getCipherSuite()));
  hpekKey = std::make_unique<mls::HPKEPrivateKey>(mls::HPKEPrivateKey::generate(getCipherSuite()));
  leaf = std::make_unique<mls::LeafNode>(
    getCipherSuite(),
    hpekKey->public_key,
    sigPrivKey->public_key,
    generateDaveMLSCredential(userId),
    getDefaultDaveMLSCapabilities(),
    mls::Lifetime::create_default(),
    mls::ExtensionList{},
    *sigPrivKey
  );
  Log::dbg("initialized leaf");
}
