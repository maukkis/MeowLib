#include "../include/user.h"

GuildUser deserializeGuildUser(nlohmann::json &j){
  return {
    .nick = j["nick"].is_null() ? "" : j["nick"],
    .avatar = j["avatar"].is_null() ? "" : j["avatar"],
    .banner = j["banner"].is_null() ? "" : j["banner"],
    .roles = j["roles"].get<std::vector<std::string>>()
  };
}

User deserializeUser(nlohmann::json& j){
  return {
    .id = j["id"],
    .avatar = j["avatar"].is_null() ? "" : j["avatar"],
    .discriminator = j["discriminator"],
    .globalName = j["global_name"].is_null() ? "" : j["global_name"],
    .username = j["username"]
  };
}
