#include "../include/emoji.h"
#include "../include/user.h"
#include "../include/nyaBot.h"
#include "../include/base64.h"
#include <nlohmann/json_fwd.hpp>
#include <optional>

namespace {
std::string& lower(std::string& a){
  std::transform(a.begin(), a.end(), a.begin(), [](char c){
    return std::tolower(c);
  });
  return a;
}
}


EmojiApiRoutes::EmojiApiRoutes(NyaBot *bot) : bot{bot} {}

std::expected<Emoji, Error> EmojiApiRoutes::createApplicationEmoji(const std::string_view name, const Attachment& file){
  nlohmann::json j;
  j["name"] = name;
  std::string type = file.name.substr(file.name.rfind(".")+1);
  std::string dataUri = "data:image/" + lower(type) + ";base64,";
  j["image"] = dataUri + encodeB64(file.data);
  auto res = bot->rest.post(std::format("https://discord.com/api/v10/applications/{}/emojis", bot->api.appId), j.dump());
  if(!res.has_value() || res->second != 201){
    Log::error("failed to create a new application emoji" + std::to_string(res->second) + 
               res.value_or(std::make_pair("meowHttp error", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return deserializeEmoji(nlohmann::json::parse(res->first));
}


std::expected<Emoji, Error> EmojiApiRoutes::modifyApplicationEmoji(const std::string_view id, const std::string_view name){
  nlohmann::json j;
  j["name"] = name;
  auto res = bot->rest.patch(std::format("https://discord.com/api/v10/applications/{}/emojis/{}", bot->api.appId, id), j.dump());
  if(!res.has_value() || res->second != 200){
    Log::error("failed to modify an application emoji" +
               res.value_or(std::make_pair("meowHttp error", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return deserializeEmoji(nlohmann::json::parse(res->first));
}

void EmojiApiRoutes::deleteApplicationEmoji(const std::string_view id){
  auto res = bot->rest.deletereq(std::format("https://discord.com/api/v10/applications/{}/emojis/{}", bot->api.appId, id));
  if(!res.has_value() || res->second != 204){
    Log::error("failed to delete an application emoji" +
               res.value_or(std::make_pair("meowHttp error", 0)).first);
  }
}


std::expected<Emoji, Error> EmojiApiRoutes::getApplicationEmoji(const std::string_view id){
  auto res = bot->rest.get(std::format("https://discord.com/api/v10/applications/{}/emojis/{}", bot->api.appId, id));
  if(!res.has_value() || res->second != 200){
    Log::error("failed to get an application emoji" + 
               res.value_or(std::make_pair("meowHttp error", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return deserializeEmoji(nlohmann::json::parse(res->first));
}


std::expected<std::unordered_map<std::string, Emoji>, Error> EmojiApiRoutes::listApplicationEmojis(){
  auto res = bot->rest.get(std::format("https://discord.com/api/v10/applications/{}/emojis", bot->api.appId));
  if(!res.has_value() || res->second != 200){
    Log::error("failed to get an application emoji" + 
               res.value_or(std::make_pair("meowHttp error", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  auto j = nlohmann::json::parse(res->first);
  std::unordered_map<std::string, Emoji> items;
  for(const auto& a : j["items"]){
    auto e = deserializeEmoji(a);
    items.insert({e.id, std::move(e)});
  }
  return items;
}

nlohmann::json serializeEmoji(const Emoji& e){
  nlohmann::json j;
  j["name"] = e.name;
  j["id"] = e.id;
  if(e.animated)
    j["animated"] = *e.animated;
  return j;
}

Emoji deserializeEmoji(const nlohmann::json& j){
  return {
    .id = j["id"],
    .name = j["name"],
    .animated = j.contains("animated") ? std::make_optional(j["animated"].get<bool>()) : std::nullopt,
    .user = j.contains("user") ? std::make_optional(deserializeUser(j["user"])) : std::nullopt
  };
}
