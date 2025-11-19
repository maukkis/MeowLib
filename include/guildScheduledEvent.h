#ifndef _INCLUDE_GUILD_SCHEDULED_EVENT_H
#define _INCLUDE_GUILD_SCHEDULED_EVENT_H
#include <string>
#include <optional>
#include "user.h"
#include <nlohmann/json_fwd.hpp>

enum class ScheduledEventPrivacyLevel {
  GUILD_ONLY = 2
};

enum class EventStatus {
  SCHEDULED = 1,
  ACTIVE,
  COMPLETED,
  CANCELED,
};

enum class EventEntityTypes {
  STAGE_INSTANCE = 1,
  VOICE,
  EXTRERNAL,
};

struct GuildScheduledEventEntityMetadata {
  std::optional<std::string> location = std::nullopt;
};

struct GuildScheduledEvent {
  GuildScheduledEvent() = default;
  GuildScheduledEvent(const nlohmann::json& j);
  std::string id;
  std::string guildId;
  std::optional<std::string> channelId = std::nullopt;
  // "creator_id will be null and creator will not be included for events created before October 25th, 2021,
  // when the concept of creator_id was introduced and tracked."
  // -- discord api docs
  std::optional<std::string> creatorId = std::nullopt;
  std::string name;
  std::optional<std::string> description = std::nullopt;
  //ISO8601 timestamp you can convert it to chrono timepoint with ISO8601ToTimepoint
  std::string scheduledStartTime;
  std::optional<std::string> scheduledEndTime = std::nullopt;
  ScheduledEventPrivacyLevel privacyLevel{};
  EventStatus status{};
  EventEntityTypes entityType{};
  std::optional<std::string> entityId = std::nullopt;
  std::optional<GuildScheduledEventEntityMetadata> entityMetadata = std::nullopt;
  std::optional<User> creator;
  std::optional<int> userCount = std::nullopt;
  std::optional<std::string> image;


};

#endif
