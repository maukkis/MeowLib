#ifndef _INCLUDE_VOICE_DAVE_DAVE_H
#define _INCLUDE_VOICE_DAVE_DAVE_H
#include <mlspp/mls/state.h>

inline static constexpr int daveVersion = 1;

class Dave {
public:
  void initState();
private:
  void initLeaf();
  std::optional<mls::State> currentState;
  std::optional<mls::State> pendingState;
};

#endif
