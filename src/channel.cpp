#include "../include/channel.h"

Channel deserializeChannel(const nlohmann::json& j) {
  Channel a;
  a.id = j["id"];
  a.type = static_cast<ChannelType>(j["type"]);
  if(j.contains("guild_id"))
    a.guildId = j["guild_id"];
  if(j.contains("position"))
    a.position = j["position"];
  if(j.contains("name") && !j["name"].is_null())
    a.name = j["name"];
  if(j.contains("topic") && !j["topic"].is_null())
    a.topic = j["topic"];
  return a;
}
