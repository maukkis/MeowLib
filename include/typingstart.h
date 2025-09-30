#ifndef _INCLUDE_TYPINGSTART_H
#define _INCLUDE_TYPINGSTART_H
#include "user.h"
#include "channel.h"
#include <optional>
#include <string>

struct TypingStart {
  TypingStart(const nlohmann::json& j);
  std::string channelId;
  std::optional<std::string> guildId;
  std::string userId;
  int timestamp{};
  std::optional<User> member;
};



#endif
