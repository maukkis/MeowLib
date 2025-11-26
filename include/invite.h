#ifndef _INCLUDE_INVITE_H
#define _INCLUDE_INVITE_H
#include <nlohmann/json_fwd.hpp>
#include "guild.h"
#include "guildScheduledEvent.h"
#include <string>


enum class InviteTypes {
  GUILD,
  GROUP_DM,
  FRIEND,
};

struct Invite {
  Invite() = default;
  Invite(const nlohmann::json& j);
  InviteTypes type{};
  std::string code;
  std::optional<Guild> guild = std::nullopt;
  std::optional<Channel> channel = std::nullopt;
  std::optional<User> inviter = std::nullopt;
  std::optional<int> approximatePresenceCount = std::nullopt;
  std::optional<int> approximateMemberCount = std::nullopt;
  std::optional<std::string> expiresAt = std::nullopt;
  std::optional<GuildScheduledEvent> guildScheduledEvent = std::nullopt;
  std::optional<int> flags = std::nullopt;
};


#endif
