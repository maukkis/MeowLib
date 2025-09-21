#include "../include/guild.h"
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include "../include/nyaBot.h"

Guild deserializeGuild(const nlohmann::json& j){
  return {
    .id = j["id"],
    .name = j["name"],
    .icon = j["icon"].is_null() ? std::nullopt : std::make_optional(j["icon"]),
    .ownerId = j.contains("owner_id") ? std::make_optional(j["owner_id"]) : std::nullopt
  };
}

GuildApiRoutes::GuildApiRoutes(NyaBot *bot){
  this->bot = bot;
}

std::expected<Guild, Error> GuildApiRoutes::get(const std::string_view id){
  auto res = bot->rest.get(std::format("https://discord.com/api/v10/guilds/{}", id));
  if(!res.has_value() || res->second != 200){
    Log::error("failed to get guild\n" + res.value_or(std::make_pair("", 0)).first);
    return {};
  }
  return deserializeGuild(nlohmann::json::parse(res->first));
}

std::expected<Guild, Error> GuildApiRoutes::getPreview(const std::string_view id){
  auto res = bot->rest.get(std::format("https://discord.com/api/v10/guilds/{}/preview", id));
  if(!res.has_value() || res->second != 200){
    Log::error("failed to get guild preview\n" + res.value_or(std::make_pair("", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return deserializeGuild(nlohmann::json::parse(res->first));
}
