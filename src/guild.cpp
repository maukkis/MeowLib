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
    .guildId = j.contains("guild_id") && !j["guild_id"].is_null() ? std::make_optional(j["guild_id"]) : std::nullopt,
    .reason = !j["reason"].is_null() ? std::make_optional(j["reason"]) : std::nullopt,
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
  return getReq(std::format(APIURL "/guilds/{}", id))
  .and_then([](std::expected<std::string, Error> a){
    return std::expected<Guild, Error>(deserializeGuild(nlohmann::json::parse(a.value())));
  });
}

std::expected<GuildPreview, Error> GuildApiRoutes::getPreview(const std::string_view id){
  return getReq(std::format(APIURL "/guilds/{}/preview", id))
  .and_then([](std::expected<std::string, Error> a){
    return std::expected<GuildPreview, Error>(deserializeGuildPreview(nlohmann::json::parse(a.value())));
  });
}


std::expected<std::vector<GuildBan>, Error> GuildApiRoutes::getGuildBans(const std::string_view id){
  return getReq(std::format(APIURL "/guilds/{}/bans", id))
  .and_then([](std::expected<std::string, Error> a){
    std::vector<GuildBan> bans;
    nlohmann::json j = nlohmann::json::parse(a.value());
    for(const auto& c : j)
      bans.emplace_back(deserializeGuildBan(c));
    return std::expected<std::vector<GuildBan>, Error>(std::move(bans));
  });
}
std::expected<GuildBan, Error> GuildApiRoutes::getGuildBan(const std::string_view guildId,
                                                           const std::string_view userId)
{
  return getReq(std::format(APIURL "/guilds/{}/bans/{}", guildId, userId))
  .and_then([](std::expected<std::string, Error> a){
    return std::expected<GuildBan, Error>(deserializeGuildBan(nlohmann::json::parse(a.value())));
  });

}
std::expected<std::nullopt_t, Error> GuildApiRoutes::createGuildBan(const std::string_view guildId,
                                                                    const std::string_view userId,
                                                                    const std::optional<int> dms,
                                                                    const std::optional<std::string>& reason)
{
  nlohmann::json j;
  if(dms)
    j["delete_messages_seconds"] = *dms;

  auto res = bot->rest.put(std::format(APIURL "/guilds/{}/bans/{}", guildId, userId), j.dump(), reason.has_value() ? 
    std::make_optional(std::vector<std::string>{"x-audit-log-reason: " + *reason }) : std::nullopt);
  if(!res.has_value() || res->second != 204){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to ban member from guild");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}
std::expected<std::nullopt_t, Error> GuildApiRoutes::removeGuildBan(const std::string_view guildId,
                                                                    const std::string_view userId,
                                                                    const std::optional<std::string>& reason)
{
  auto res = bot->rest.deletereq(std::format(APIURL "/guilds/{}/bans/{}", guildId, userId), reason.has_value() ? 
    std::make_optional(std::vector<std::string>{"x-audit-log-reason: " + *reason }) : std::nullopt);
  if(!res.has_value() || res->second != 204){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to remove ban from member from guild");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}


std::expected<std::nullopt_t, Error> GuildApiRoutes::removeGuildMember(const std::string_view guildId,
                                                                       const std::string_view userId,
                                                                       const std::optional<std::string>& auditLogReason )
{
  auto res = bot->rest.deletereq(std::format(APIURL "/guilds/{}/members/{}", guildId, userId), auditLogReason.has_value() ? 
    std::make_optional(std::vector<std::string>{"x-audit-log-reason: " + *auditLogReason }) : std::nullopt);
  if(!res.has_value() || res->second != 204){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to remove guild member");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}


std::expected<User, Error> GuildApiRoutes::modifyGuildMember(const std::string_view guildId,
                                                             const std::string_view userId,
                                                             const GuildUser& user,
                                                             const std::optional<std::string>& reason)
{
  auto res = bot->rest.patch(std::format(APIURL "/guilds/{}/members/{}", guildId, userId),
    serializeGuildUser(user).dump(),
    reason.has_value() ? std::make_optional(std::vector<std::string>{"x-audit-log-reason: " + *reason }) : std::nullopt);
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to modify guild member");
    err.printErrors();
    return std::unexpected(err);
  }
  auto j = nlohmann::json::parse(res->first);
  auto u = deserializeUser(j["user"]);
  u.guild = deserializeGuildUser(j);
  return u;
}


std::expected<std::string, Error> GuildApiRoutes::getReq(const std::string& endpoint){
  auto res = bot->rest.get(endpoint);
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to get guild");
    err.printErrors();
    return std::unexpected(err);
  }
  return res->first;
}
