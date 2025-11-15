#include "../include/channel.h"
#include "../include/nyaBot.h"

Channel::Channel(const nlohmann::json& j) {
  id = j["id"];
  type = static_cast<ChannelType>(j["type"]);
  if(j.contains("guild_id"))
    guildId = j["guild_id"];
  if(j.contains("position"))
    position = j["position"];
  if(j.contains("name") && !j["name"].is_null())
    name = j["name"];
  if(j.contains("topic") && !j["topic"].is_null())
    topic = j["topic"];
  if(j.contains("newly_created")){
    newlyCreated = j["newly_created"];
  }
  if(j.contains("last_message_id") && !j["last_message_id"].is_null())
    lastMessageId = j["last_message_id"];
  if(j.contains("parent_id") && !j["parent_id"].is_null())
    parentId = j["parent_id"];
  if(j.contains("last_pin_timestamp") && !j["last_pin_timestamp"].is_null())
    lastPinTimestamp = j["last_pin_timestamp"];
  if(j.contains("nsfw"))
    nsfw = j["nsfw"];
  if(j.contains("thread_metadata"))
    threadMetadata = ThreadMetadata(j["thread_metadata"]);
}



nlohmann::json Channel::generate() const {
  nlohmann::json j;
  if(!id.empty())
    j["id"] = id;

  j["type"] = static_cast<int>(type);

  if(guildId)
    j["guild_id"] = *guildId;
  
  if(position)
    j["position"] = *position;

  if(name)
    j["name"] = *name;

  if(topic)
    j["topic"] = *topic;
  return j;
}

ChannelApiRoutes::ChannelApiRoutes(NyaBot *bot) : cache{"/channels", &bot->rest}, bot{bot} {}


std::expected<Channel, Error> ChannelApiRoutes::get(const std::string_view id){
  return cache.get(std::string(id));
}


std::expected<Channel, Error> ChannelApiRoutes::modify(const std::string_view id, const Channel& ch){
  auto res = bot->rest.patch(std::format(APIURL "/channels/{}", id), ch.generate().dump());
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to modify channel");
    err.printErrors();
    return std::unexpected(err);
  }
  Channel a(nlohmann::json::parse(res->first));
  cache.insert(a.id, a);
  return a;
}


std::expected<Channel, Error> ChannelApiRoutes::close(const std::string_view id){
  auto res = bot->rest.deletereq(std::format(APIURL "/channels/{}", id));
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to modify channel");
    err.printErrors();
    return std::unexpected(err);
  }
  return Channel(nlohmann::json::parse(res->first));
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
