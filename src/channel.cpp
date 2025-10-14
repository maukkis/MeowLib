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
  if(j.contains("newly_created")){
    a.newlyCreated = j["newly_created"];
  }
  if(j.contains("last_message_id") && !j["last_message_id"].is_null())
    a.lastMessageId = j["last_message_id"];
  if(j.contains("parent_id") && !j["parent_id"].is_null())
    a.parentId = j["parent_id"];
  if(j.contains("last_pin_timestamp") && !j["last_pin_timestamp"].is_null())
    a.lastPinTimestamp = j["last_pin_timestamp"];
  if(j.contains("nsfw"))
    a.nsfw = j["nsfw"];
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


std::expected<std::nullopt_t, Error> ChannelApiRoutes::joinThread(const std::string_view id){
  auto res = bot->rest.put(std::format(APIURL "/channels/{}/thread-members/@me", id));
  if(!res.has_value() || res->second != 204){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to join thread");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}
