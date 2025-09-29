#include "../include/guild.h"
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include "../include/nyaBot.h"


UnavailableGuild deserializeUnavailableGuild(const nlohmann::json& j){
  return {
    .id = j["id"],
    .unavailable = j["unavailable"]
  };
}

GuildBan deserializeGuildBan(const nlohmann::json& j){
  return {
    .guildId = j["guild_id"],
    .user = deserializeUser(j["user"])
  };
}

Guild deserializeGuild(const nlohmann::json& j){
  Guild g;
  g.id = j["id"];
  g.name = j["name"];
  if(!j["icon"].is_null())
    g.icon = j["icon"];
  if(j.contains("icon_hash") && !j["icon_hash"].is_null())
    g.iconHash = j["icon_hash"];
  if(!j["splash"].is_null())
    g.splash = j["splash"];
  if(!j["discovery_splash"].is_null())
    g.discoverySplash = j["discovery_splash"];
  if(j.contains("owner"))
    g.owner = j["owner"];
  g.ownerId = j["owner_id"];
  if(j.contains("permissions"))
    g.permissions = j["permissions"];
  for(const auto& a : j["roles"])
    g.roles.emplace_back(a);
  for(const auto& a : j["emojis"])
    g.emojis.emplace_back(deserializeEmoji(a));
  if(!j["banner"].is_null())
    g.banner = j["banner"];
  g.nsfwLevel = j["nsfw_level"].get<int>();
  return g;
}

GuildPreview deserializeGuildPreview(const nlohmann::json& j){
  GuildPreview g;
  g.id = j["id"];
  g.name = j["name"];
  if(!j["icon"].is_null())
    g.icon = j["icon"];
  if(!j["splash"].is_null())
    g.splash = j["splash"];
  if(!j["discovery_splash"].is_null())
    g.discoverySplash = j["discovery_splash"];
  for(const auto& e : j["emojis"])
    g.emojis.emplace_back(deserializeEmoji(e));
  g.features = j["features"].get<std::vector<std::string>>();
  g.approximateMemberCount = j["approximate_member_count"];
  g.approximatePresenceCount = j["approximate_presence_count"];
  if(!j["description"].is_null())
    g.description = j["description"];
  return g;
}


GuildApiRoutes::GuildApiRoutes(NyaBot *bot){
  this->bot = bot;
}

std::expected<Guild, Error> GuildApiRoutes::get(const std::string_view id){
  auto res = bot->rest.get(std::format(APIURL "/guilds/{}", id));
  if(!res.has_value() || res->second != 200){
    Log::error("failed to get guild\n" + res.value_or(std::make_pair("", 0)).first);
    return {};
  }
  return deserializeGuild(nlohmann::json::parse(res->first));
}

std::expected<GuildPreview, Error> GuildApiRoutes::getPreview(const std::string_view id){
  auto res = bot->rest.get(std::format(APIURL "/guilds/{}/preview", id));
  if(!res.has_value() || res->second != 200){
    Log::error("failed to get guild preview\n" + res.value_or(std::make_pair("", 0)).first);
    return std::unexpected(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return deserializeGuildPreview(nlohmann::json::parse(res->first));
}
