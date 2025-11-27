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

enum class InviteTargetTypes {
  STREAM = 1,
  EMBEDDED_APPLICATION,
};


struct Invite {
  Invite() = default;
  Invite(const nlohmann::json& j);
  InviteTypes type{};
  std::string code;
  std::optional<Guild> guild = std::nullopt;
  std::optional<Channel> channel = std::nullopt;
  std::optional<User> inviter = std::nullopt;
  std::optional<InviteTargetTypes> targetType = std::nullopt;
  std::optional<User> targetUser = std::nullopt;
  std::optional<int> approximatePresenceCount = std::nullopt;
  std::optional<int> approximateMemberCount = std::nullopt;
  std::optional<std::string> expiresAt = std::nullopt;
  std::optional<GuildScheduledEvent> guildScheduledEvent = std::nullopt;
  std::optional<int> flags = std::nullopt;
};

struct InviteCreateEvent {
  InviteCreateEvent() = default;
  InviteCreateEvent(const nlohmann::json& j);
  std::string code;
  std::string createdAt;
  std::optional<std::string> guildId = std::nullopt;
  std::optional<std::string> channelId = std::nullopt;
  std::optional<User> inviter = std::nullopt;
  int maxAge{};
  int maxUses{};
  std::optional<InviteTargetTypes> targetType = std::nullopt;
  std::optional<User> targetUser = std::nullopt;
  bool temporary{};
  int uses{};
  std::optional<std::string> expiresAt = std::nullopt;
};

struct InviteDeleteEvent {
  InviteDeleteEvent() = default;
  InviteDeleteEvent(const nlohmann::json& j);
  std::string channelId;
  std::optional<std::string> guildId = std::nullopt;
  std::string code;
};

class InviteApiRoutes {
public:
  InviteApiRoutes(NyaBot *bot);
  std::expected<Invite, Error> get(const std::string_view code, const bool withCounts = false);
  std::expected<Invite, Error> remove(const std::string_view code);
private:
  NyaBot *bot;
};

#endif
