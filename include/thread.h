#ifndef _INCLUDE_THREAD_H
#define _INCLUDE_THREAD_H
#include "channel.h"
#include "user.h"
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
#include <optional>



struct ThreadMember {
  ThreadMember() = default;
  ThreadMember(const nlohmann::json& j);
  std::optional<std::string> id;
  std::optional<std::string> userId;
  std::string joinTimestamp;
  int flags{};
  std::optional<User> member;
  // only for thread member update event
  std::optional<std::string> guildId;
};

struct ThreadListSync {
  ThreadListSync() = default;
  ThreadListSync(const nlohmann::json& j);
  std::string guildId;
  std::vector<std::string> channelIds;
  std::vector<Channel> threads;
  std::vector<ThreadMember> members;
};

struct ThreadMembersUpdate {
  ThreadMembersUpdate() = default;
  ThreadMembersUpdate(const nlohmann::json& j);
  std::string id;
  std::string guildId;
  int memberCount{};
  std::vector<ThreadMember> addedMembers;
  std::vector<std::string> removedMemberIds;
};

#endif
