#include "../include/guild.h"
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <format>
#include "../include/helpers.h"
#include "../include/nyaBot.h"
#include "../include/eventCodes.h"


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

std::future<std::vector<User>> NyaBot::requestGuildMembers(const std::string_view guildId,
                                                           const std::string_view query,
                                                           int limit)
{
  nlohmann::json j;
  j["op"] = RequestGuildMember;
  auto nonce = generate32ByteASCIINonce();
  j["d"] = nlohmann::json::object();
  j["d"]["nonce"] = nonce;
  j["d"]["guild_id"] = guildId;
  j["d"]["query"] = query;
  j["d"]["limit"] = limit;
  std::promise<std::vector<User>> promise;
  auto fut = promise.get_future();
  std::unique_lock<std::mutex> lock(guildMemberChunkmtx);
  guildMembersChunkTable.insert({nonce, GuildMemberRequestData{.callback = [pro = std::move(promise)](std::vector<User> a) mutable {
    pro.set_value(std::move(a));
  }}});
  queue.addToQueue(j.dump());
  return fut;
}


Guild deserializeGuild(const nlohmann::json& j){
  Guild g;
  g.id = j["id"];
  g.name = j["name"];
  if(!j["icon"].is_null())
    g.icon = j["icon"];
  if(j.contains("icon_hash") && !j["icon_hash"].is_null())
    g.iconHash = j["icon_hash"];
  if(j.contains("splash") && !j["splash"].is_null())
    g.splash = j["splash"];
  if(j.contains("discovery_splash") && !j["discovery_splash"].is_null())
    g.discoverySplash = j["discovery_splash"];
  if(j.contains("owner"))
    g.owner = j["owner"];
  if(j.contains("owner_id"))
    g.ownerId = j["owner_id"];
  if(j.contains("permissions"))
    g.permissions = j["permissions"];
  if(!j["banner"].is_null())
    g.banner = j["banner"];
  if(j.contains("nsfw_level"))
    g.nsfwLevel = j["nsfw_level"].get<int>();

  if(j.contains("roles")){
    for(const auto& a : j["roles"])
      g.roles.emplace_back(a);
  }
  if(j.contains("emojis")){
    for(const auto& a : j["emojis"])
      g.emojis.emplace_back(deserializeEmoji(a));
  }

  return g;
}

GuildPreview deserializeGuildPreview(const nlohmann::json& j){
  GuildPreview g;
  g.id = j["id"];
  g.name = j["name"];
  if(!j["icon"].is_null())
    g.icon = j["icon"];
  if(j.contains("splash") && !j["splash"].is_null())
    g.splash = j["splash"];
  if(j.contains("discovery_splash") && !j["discovery_splash"].is_null())
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
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to get guild");
    err.printErrors();
    return std::unexpected(err);
  }
  return deserializeGuild(nlohmann::json::parse(res->first));
}

std::expected<GuildPreview, Error> GuildApiRoutes::getPreview(const std::string_view id){
  auto res = bot->rest.get(std::format(APIURL "/guilds/{}/preview", id));
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to get guild preview");
    err.printErrors();
    return std::unexpected(err);
  }
  return deserializeGuildPreview(nlohmann::json::parse(res->first));
}
