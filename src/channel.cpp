#include "../include/channel.h"
#include "../include/nyaBot.h"

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



nlohmann::json serializeChannel(const Channel& a){
  nlohmann::json j;
  if(!a.id.empty())
    j["id"] = a.id;

  j["type"] = static_cast<int>(a.type);

  if(a.guildId)
    j["guild_id"] = *a.guildId;
  
  if(a.position)
    j["position"] = *a.position;

  if(a.name)
    j["name"] = *a.name;

  if(a.topic)
    j["topic"] = *a.topic;
  return j;
}

ChannelApiRoutes::ChannelApiRoutes(NyaBot *bot) : bot{bot} {}

std::expected<Channel, Error> ChannelApiRoutes::modify(const std::string_view id, const Channel& ch){
  auto res = bot->rest.patch(std::format(APIURL "/channels/{}", id), serializeChannel(ch).dump());
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to modify channel");
    err.printErrors();
    return std::unexpected(err);
  }
  return deserializeChannel(nlohmann::json::parse(res->first));
}


std::expected<Channel, Error> ChannelApiRoutes::close(const std::string_view id){
  auto res = bot->rest.deletereq(std::format(APIURL "/channels/{}", id));
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to modify channel");
    err.printErrors();
    return std::unexpected(err);
  }
  return deserializeChannel(nlohmann::json::parse(res->first));
}
