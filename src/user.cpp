#include "../include/user.h"
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string_view>
#include "../include/nyaBot.h"
#include "../include/helpers.h"

UserApiRoutes::UserApiRoutes(NyaBot *bot)
  : bot{bot}{}

std::expected<User, Error> UserApiRoutes::getUser(const std::string_view id){
  return getReq(std::format(APIURL "/users/{}", id))
    .and_then([](std::expected<std::string, Error> a){
      return std::expected<User, Error>(deserializeUser(nlohmann::json::parse(a.value())));
    });
}

std::expected<User, Error> UserApiRoutes::getCurrentUser(){
  return getReq(APIURL "/users/@me")
    .and_then([](std::expected<std::string, Error> a){
      return std::expected<User, Error>(deserializeUser(nlohmann::json::parse(a.value())));
    });
}

std::expected<Channel, Error> UserApiRoutes::createDM(const std::string_view id){
  nlohmann::json j;
  j["recipient_id"] = id;
  auto res = bot->rest.post(
    std::format("https://discord.com/api/v10/users/@me/channels"),
    j.dump()
  );
  if(!res.has_value() || res->second != 200){
    Log::error("failed to create dm channel"
             + res.value_or(std::make_pair("", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  auto i = nlohmann::json::parse(res->first);
  return deserializeChannel(i);
}

std::expected<User, Error> UserApiRoutes::modifyCurrentUser(std::optional<std::string> username,
                                               std::optional<Attachment> avatar,
                                               std::optional<Attachment> banner)
{
  nlohmann::json j;
  if(username)
    j["username"] = *username;
  if(avatar)
    j["avatar"] = attachmentToDataUri(*avatar);
  if(banner)
    j["banner"] = attachmentToDataUri(*banner);
  auto res = bot->rest.patch(APIURL "/users/@me", j.dump());
  if(!res.has_value() || res->second != 200){
    Log::error("failed to modify current user"
             + res.value_or(std::make_pair("", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return deserializeUser(nlohmann::json::parse(res->first));
}
 
std::expected<std::string, Error> UserApiRoutes::getReq(const std::string& endpoint){
  auto res = bot->rest.get(endpoint);
  if(!res.has_value() || res->second != 200){
    Log::error("failed to fetch user "
             + res.value_or(std::make_pair("", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return res->first;
}

GuildUser deserializeGuildUser(const nlohmann::json &j){
  return {
    .nick = j["nick"].is_null() ? "" : j["nick"],
    .avatar = j["avatar"].is_null() ? "" : j["avatar"],
    .banner = j["banner"].is_null() ? "" : j["banner"],
    .roles = j["roles"].get<std::vector<std::string>>()
  };
}

User deserializeUser(const nlohmann::json& j){
  return {
    .id = j["id"],
    .avatar = j["avatar"].is_null() ? "" : j["avatar"],
    .discriminator = j["discriminator"],
    .globalName = j["global_name"].is_null() ? "" : j["global_name"],
    .username = j["username"],
    .bot = j.contains("bot") ? j["bot"].get<bool>() : false,
    .primaryGuild = j.contains("primary_guild") &&
      !j["primary_guild"].is_null() &&
      !j["primary_guild"]["identity_guild_id"].is_null() ?
        std::make_optional(deserializePrimaryGuild(j["primary_guild"]))
      : std::nullopt
  };
}

PrimaryGuild deserializePrimaryGuild(const nlohmann::json& j){
  return {
    .identityGuildId = j["identity_guild_id"],
    .identityEnabled = j["identity_enabled"].is_null() ?
        std::nullopt 
      : std::make_optional(j["identity_enabled"].get<bool>()),
    .tag = j["tag"],
    .badge = j["badge"]
  };
}

