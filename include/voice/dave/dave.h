#ifndef _INCLUDE_VOICE_DAVE_DAVE_H
#define _INCLUDE_VOICE_DAVE_DAVE_H
#include <cstdint>
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
};

class Dave {
public:
  Dave(const std::string& userId, int groupId = 1);
  DaveProcessInfo processDavePayload(const std::string_view payload);
private:
  void addToLut(std::function<std::optional<std::string>(const std::string_view)> f, VoiceOpcodes opc);
  std::optional<std::string> processExternalSender(const std::string_view);
  std::optional<std::string> processProposals(const std::string_view);
  void initLeaf(const std::string& userId);
  std::optional<mls::State> currentState;
  std::optional<mls::State> pendingState;
  std::optional<mls::State> cachedState;
  std::optional<mls::LeafNode> leaf;
  std::optional<mls::ExternalSender> externalSender;
  mls::bytes_ns::bytes groupId;
  mls::HPKEPrivateKey hpekKey;
  mls::SignaturePrivateKey sigPrivKey;
  std::array<std::function<std::optional<std::string>(const std::string_view)>, 11> daveLut{nullptr};
};

#endif
