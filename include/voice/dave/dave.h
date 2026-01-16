#ifndef _INCLUDE_VOICE_DAVE_DAVE_H
#define _INCLUDE_VOICE_DAVE_DAVE_H
#include <cstdint>
#include <meowHttp/websocket.h>
#include <mlspp/mls/messages.h>
#include <mlspp/mls/state.h>
#include <vector>
#include <array>
#include <functional>
#include "../voiceOpcodes.h"

#define todo(x) Log::error(x);\
                std::terminate();
inline static constexpr int daveVersion = 1;

struct DaveProcessInfo {
  std::optional<std::string> toSend;
  uint16_t seq;
  meowWs::opcodes opcode;
};


bool isDaveEvent(int opcode);
bool isBinaryEvent(int opcode);

class Dave {
public:
  Dave(const std::string& userId, uint64_t groupId);
  DaveProcessInfo processDavePayload(const std::string_view payload, bool json = false);
  std::string getKeyPackagePayload();
private:
  void addToLut(std::function<std::optional<std::string>(const std::string_view)> f, VoiceOpcodes opc);
  std::optional<std::string> processExternalSender(const std::string_view);
  std::optional<std::string> processProposals(const std::string_view);
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
};

#endif
