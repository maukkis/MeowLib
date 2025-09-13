#include "../include/user.h"
#include <nlohmann/json_fwd.hpp>
#include <string_view>
#include "../include/nyaBot.h"

UserApiRoutes::UserApiRoutes(NyaBot *bot)
  : bot{bot}{}

User UserApiRoutes::getUser(const std::string_view id){
  auto res = bot->rest.get(
    std::format("https://discord.com/api/v10/users/{}", id)
  );
  if(!res.has_value() || res->second != 200){
    Log::error("failed to fetch user "
             + res.value_or(std::make_pair("", 0)).first);
    return User();
  }
  auto j = nlohmann::json::parse(res->first);
  return deserializeUser(j);
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
    .username = j["username"]
  };
}
