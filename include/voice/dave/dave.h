#ifndef _INCLUDE_VOICE_DAVE_DAVE_H
#define _INCLUDE_VOICE_DAVE_DAVE_H
#include <cstdint>
#include <meowHttp/websocket.h>
#include <mlspp/mls/messages.h>
#include <mlspp/mls/state.h>
#include <array>
#include <functional>
#include <unordered_set>
#include <vector>
#include "../voiceOpcodes.h"
#include "encryptor.h"

#define todo(x) Log::error(x);\
                std::terminate();
inline static constexpr int daveVersion = 1;

struct DaveProcessInfo {
  std::optional<std::string> toSend;
  std::optional<uint16_t> seq;
  meowWs::opcodes opcode;
};

struct TransitionInfo {
  int transitionId{};
  int protocolVersion{};
};

bool isDaveEvent(int opcode);
bool isBinaryEvent(int opcode);

class Dave {
public:
  Dave(const std::string& userId, uint64_t groupId);
  bool ready();
  void addUsers(const std::vector<std::string>& a);
  void removeUsers(const std::vector<std::string>& a);
  DaveProcessInfo processDavePayload(const std::string_view payload, bool json = false);
  std::string getKeyPackagePayload();
  std::optional<Encryptor> encryptor;
private:
  void createEncryptor();
  std::vector<uint8_t> getKeyForGeneration(uint32_t generation);
  void addToLut(std::function<std::optional<std::string>(const std::string_view)> f, VoiceOpcodes opc);
  std::optional<std::string> processExternalSender(const std::string_view);
  std::optional<std::string> processProposals(const std::string_view);
  std::optional<std::string> processCommitTransition(const std::string_view);
  std::optional<std::string> processWelcome(const std::string_view);
  std::optional<std::string> prepareTransition(const std::string_view);
  std::optional<std::string> executeTransition(const std::string_view);
  bool isValidProposal(const mls::ValidatedContent& a);
  bool isValidWelcomeState();
  void initLeaf(const std::string& userId);
  std::optional<mls::State> currentState;
  std::optional<mls::State> pendingState;
  std::optional<mls::State> cachedState;
  std::optional<mls::State> commitState;
  // why are all these unique ptrs?
  // turns out mlspp is a stupid fucking library this was the only way they started working
  std::unique_ptr<mls::LeafNode> leaf;
  std::unique_ptr<mls::KeyPackage> keyPackage;
  std::unique_ptr<mls::HPKEPrivateKey> joinInitKey;
  std::optional<mls::ExternalSender> externalSender;
  mls::bytes_ns::bytes groupId;
  std::unique_ptr<mls::HPKEPrivateKey> hpekKey;
  std::unique_ptr<mls::SignaturePrivateKey> sigPrivKey;
  std::array<std::function<std::optional<std::string>(const std::string_view)>, 11> daveLut{nullptr};
  std::unordered_set<uint64_t> users;
  std::string botId;
  mls::HashRatchet keyratchet;
  std::optional<TransitionInfo> transitionInfo;
  friend Encryptor;
};

mls::CipherSuite getCipherSuite();

template<typename T>
mls::bytes_ns::bytes bytesFrom(const T& value, size_t size){
  mls::bytes_ns::bytes bytes;
  for(size_t i = 0; i < size; ++i){
    bytes.push_back(std::bit_cast<uint8_t *>(&value)[i]);
  }
  return bytes;
}

uint64_t snowflakeFromCredential(const mls::bytes_ns::bytes& val);

#endif
