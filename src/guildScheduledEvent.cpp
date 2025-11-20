#include "../include/guildScheduledEvent.h"
#include <nlohmann/json.hpp>

GuildScheduledEvent::GuildScheduledEvent(const nlohmann::json& j){
  id = j["id"];
  guildId = j["guild_id"];
  if(!j["channel_id"].is_null())
    channelId = j["channel_id"];
  if(j.contains("creator_id") && !j["creator_id"].is_null())
    creatorId = j["creator_id"];
  name = j["name"];
  if(j.contains("description") && !j["description"].is_null())
    description = j["description"];
  scheduledStartTime = j["scheduled_start_time"];
  if(!j["scheduled_end_time"].is_null())
    scheduledEndTime = j["scheduled_end_time"];
  privacyLevel = j["privacy_level"];
  status = j["status"];
  entityType = j["entity_type"];
  if(!j["entity_id"].is_null())
    entityId = j["entity_id"];
  if(!j["entity_metadata"].is_null() && j["entity_metadata"].contains("location"))
    entityMetadata = GuildScheduledEventEntityMetadata{.location = j["entity_metadata"]["location"]};
  if(j.contains("creator"))
    creator = User(j["creator"]);
  if(j.contains("user_count"))
    userCount = j["user_count"];
  if(j.contains("image") && !j["image"].is_null())
    image = j["image"];
}


nlohmann::json GuildScheduledEvent::generate() const {
  nlohmann::json j;
  if(channelId)
    j["channel_id"] = *channelId;
  if(entityMetadata)
    j["entity_metadata"]["location"] = entityMetadata->location;
  if(!name.empty())
    j["name"] = name;
  if(privacyLevel != ScheduledEventPrivacyLevel::NONE)
    j["privacy_level"] = privacyLevel;
  if(!scheduledStartTime.empty())
    j["scheduled_start_time"] = scheduledStartTime;
  if(scheduledEndTime)
    j["scheduled_end_time"] = *scheduledEndTime;
  if(description)
    j["description"] = *description;
  if(entityType != EventEntityTypes::NONE)
    j["entity_type"] = entityType;
  if(image && image->starts_with("data"))
    j["image"] = *image;
  return j;
}
