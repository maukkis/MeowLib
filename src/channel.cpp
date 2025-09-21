#include "../include/channel.h"

Channel deserializeChannel(const nlohmann::json& j) {
  return {
    .id = j["id"],
    .type = static_cast<ChannelType>(j["type"]),
    .guildId = j.contains("guild_id") ? std::make_optional(j["guild_id"]) : std::nullopt,
    .position = j.contains("position") ? std::make_optional(j["position"]) : std::nullopt,
    .name = j.contains("name") ? std::make_optional(j["name"]) : std::nullopt,
    .topic = j.contains("topic") ? std::make_optional(j["topic"]) : std::nullopt,
  };
}
