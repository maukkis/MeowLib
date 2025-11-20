#ifndef _INCLUDE_GUILD_SCHEDULED_EVENT_H
#define _INCLUDE_GUILD_SCHEDULED_EVENT_H
#include <string>
#include <optional>
#include "user.h"
#include <nlohmann/json_fwd.hpp>

enum class ScheduledEventPrivacyLevel {
  NONE = 0,
  GUILD_ONLY = 2
};

enum class EventStatus {
  NONE = 0, // this is a placeholder and if it set to this it wont be serialized
  SCHEDULED = 1,
  ACTIVE,
  COMPLETED,
  CANCELED,
};

enum class EventEntityTypes {
  NONE = 0, // this is a placeholder and if it set to this it wont be serialized
  STAGE_INSTANCE = 1,
  VOICE,
  EXTRERNAL,
};

/// @brief used in the guild scheduled event user gateway events
struct GuildScheduledEventUser {
  std::string guildScheduledEventId;
  std::string userId;
  std::string guildId;
};


struct GuildScheduledEventEntityMetadata {
  std::string location;
};

struct GuildScheduledEvent {
  GuildScheduledEvent() = default;
  GuildScheduledEvent(const nlohmann::json& j);
  nlohmann::json generate() const;
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
  ScheduledEventPrivacyLevel privacyLevel = ScheduledEventPrivacyLevel::NONE;
  EventStatus status = EventStatus::NONE;
  EventEntityTypes entityType = EventEntityTypes::NONE;
  std::optional<std::string> entityId = std::nullopt;
  std::optional<GuildScheduledEventEntityMetadata> entityMetadata = std::nullopt;
  std::optional<User> creator = std::nullopt;
  std::optional<int> userCount = std::nullopt;
  std::optional<std::string> image = std::nullopt;
};

#endif
