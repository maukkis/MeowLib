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
    .user = User(j["user"])
  };
}

MeowAsync<std::vector<User>> NyaBot::requestGuildMembers(const std::string_view guildId,
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
  std::unique_lock<std::mutex> lock(guildMemberChunkmtx);
  guildMembersChunkTable[nonce] = GuildMemberRequestTask{};
  lock.unlock();
  queue.addToQueue(j.dump());
  Log::dbg("request guild members sent off to the gateway");
  co_return co_await guildMembersChunkTable[nonce];
}


Guild::Guild(const nlohmann::json& j){
  id = j["id"];
  name = j["name"];
  if(!j["icon"].is_null())
    icon = j["icon"];
  if(j.contains("icon_hash") && !j["icon_hash"].is_null())
    iconHash = j["icon_hash"];
  if(j.contains("splash") && !j["splash"].is_null())
    splash = j["splash"];
  if(j.contains("discovery_splash") && !j["discovery_splash"].is_null())
    discoverySplash = j["discovery_splash"];
  if(j.contains("owner"))
    owner = j["owner"];
  if(j.contains("owner_id"))
    ownerId = j["owner_id"];
  if(j.contains("permissions"))
    permissions = j["permissions"];
  if(!j["banner"].is_null())
    banner = j["banner"];
  if(j.contains("nsfw_level"))
    nsfwLevel = j["nsfw_level"].get<int>();

  if(j.contains("roles")){
    for(const auto& a : j["roles"])
      roles.emplace_back(a);
  }
  if(j.contains("emojis")){
    for(const auto& a : j["emojis"])
      emojis.emplace_back(deserializeEmoji(a));
  }
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


GuildCache::GuildCache(NyaBot *bot)
  : GenericDiscordCache("/guilds", &bot->rest), bot{bot} {}


void GuildCache::insertGuildUser(const std::string& guildId, const User& a){
  if(!relatedObjects.contains(guildId)){
    std::unique_lock<std::mutex> lock(relatedObjectsmtx);
    relatedObjects[guildId];
  }
  std::unique_lock<std::mutex> lock(relatedObjects[guildId].usersmtx);
  relatedObjects[guildId].users.insert(a.id, *a.guild);
}

std::expected<User, Error> GuildCache::getGuildUser(const std::string& guildId, const std::string& id)
{
  auto user = bot->user.cache.getFromCache(id);
  if(user){
    std::unique_lock<std::mutex> lock(relatedObjects[guildId].usersmtx);
    auto guser = relatedObjects[guildId].users.get(id);
    if(!guser || hrclk::now() - guser->made > ttl){
      auto a = fetchGuildUser(guildId, id);
      return a;
    }
    user->guild = guser->object;
    return *user;
  }
  return fetchGuildUser(guildId, id);
}

std::expected<User, Error> GuildCache::fetchGuildUser(const std::string& guildId, const std::string& id)
{
  auto res = rest->get(std::format(APIURL "/guilds/{}/members/{}", guildId, id));
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to get item " + id);
    err.printErrors();
    return std::unexpected(err);
  }
  auto j = nlohmann::json::parse(res.value().first);
  User a(j["user"]);
  a.guild = GuildUser(j);
  bot->user.cache.insert(a.id, a);
  insertGuildUser(guildId, a);
  return a;
}



std::expected<std::vector<Channel>, Error> GuildCache::getGuildChannels(const std::string& guildId){
  if(!relatedObjects.contains(guildId)){
    std::unique_lock<std::mutex> lock(relatedObjectsmtx);
    relatedObjects[guildId];
  }
  auto channelIds = relatedObjects[guildId].channelIds;
  std::vector<Channel> channels;
  if(!channelIds.empty()){
    std::unique_lock<std::mutex> lock(relatedObjects[guildId].channelmtx);
    for(const auto& channelId : channelIds){
      auto ch = bot->channel.cache.getFromCache(channelId);
      if(!ch) {
        lock.unlock();
        return fetchGuildChannels(guildId);
      }
      channels.emplace_back(*ch);
    }
    return channels;
  }
  return fetchGuildChannels(guildId);
}

void GuildCache::insertGuildChannel(const Channel& a){
  if(!relatedObjects.contains(*a.guildId)){
    std::unique_lock lock(relatedObjectsmtx);
    relatedObjects[*a.guildId];
  }
  std::unique_lock lock(relatedObjects[*a.guildId].channelmtx);
  relatedObjects[*a.guildId].channelIds.emplace(a.id);
}


void GuildCache::removeGuildChannel(const std::string& guildId, const std::string& channelId){
  if(!relatedObjects.contains(guildId) || 
     !relatedObjects[guildId].channelIds.contains(channelId)) return;
  std::unique_lock lock(relatedObjects[guildId].channelmtx);
  relatedObjects[guildId].channelIds.erase(channelId);
}

std::expected<std::vector<Channel>, Error> GuildCache::fetchGuildChannels(const std::string& guildId){
  Log::dbg("cache miss from guild channels!!");
  auto res = rest->get(std::format(APIURL "/guilds/{}/channels", guildId));
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to get item " + guildId);
    err.printErrors();
    return std::unexpected(err);
  }
  std::vector<Channel> vec;
  auto j = nlohmann::json::parse(res->first);
  std::unique_lock<std::mutex> lock(relatedObjects[guildId].channelmtx);
  for(const auto& channel : j){
    Channel a(channel);
    relatedObjects[guildId].channelIds.emplace(a.id);
    bot->channel.cache.insert(a.id, a);
    vec.emplace_back(std::move(a));
  }
  return vec;
}

GuildApiRoutes::GuildApiRoutes(NyaBot *bot): cache{bot}{
  this->bot = bot;
}

std::expected<Guild, Error> GuildApiRoutes::get(const std::string_view id){
  return cache.get(std::string(id));
}

std::expected<GuildPreview, Error> GuildApiRoutes::getPreview(const std::string_view id){
  return getReq(std::format(APIURL "/guilds/{}/preview", id))
  .and_then([](std::expected<std::string, Error> a){
    return std::expected<GuildPreview, Error>(deserializeGuildPreview(nlohmann::json::parse(a.value())));
  });
}



std::expected<std::vector<Channel>, Error> GuildApiRoutes::getChannels(const std::string_view guildId){
  return cache.getGuildChannels(std::string(guildId));
}


std::expected<Channel, Error> GuildApiRoutes::createChannel(const std::string_view guildId, const Channel& a){
  auto res = bot->rest.post(std::format(APIURL "/guilds/{}/channels", guildId), a.generate().dump());
  if(!res.has_value() || res->second != 201){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to create guild channel");
    err.printErrors();
    return std::unexpected(err);
  }
  Channel b(nlohmann::json::parse(res->first));
  bot->channel.cache.insert(b.id, b);
  return b;
}


std::expected<std::vector<GuildBan>, Error> GuildApiRoutes::getBans(const std::string_view id){
  return getReq(std::format(APIURL "/guilds/{}/bans", id))
  .and_then([](std::expected<std::string, Error> a){
    std::vector<GuildBan> bans;
    nlohmann::json j = nlohmann::json::parse(a.value());
    for(const auto& c : j)
      bans.emplace_back(deserializeGuildBan(c));
    return std::expected<std::vector<GuildBan>, Error>(std::move(bans));
  });
}
std::expected<GuildBan, Error> GuildApiRoutes::getBan(const std::string_view guildId,
                                                           const std::string_view userId)
{
  return getReq(std::format(APIURL "/guilds/{}/bans/{}", guildId, userId))
  .and_then([](std::expected<std::string, Error> a){
    return std::expected<GuildBan, Error>(deserializeGuildBan(nlohmann::json::parse(a.value())));
  });

}
std::expected<std::nullopt_t, Error> GuildApiRoutes::createBan(const std::string_view guildId,
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
std::expected<std::nullopt_t, Error> GuildApiRoutes::removeBan(const std::string_view guildId,
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

std::expected<User, Error> GuildApiRoutes::getMember(const std::string_view guildId,
                                                     const std::string_view id)
{
  return cache.getGuildUser(std::string(guildId), std::string(id));
}


std::expected<std::nullopt_t, Error> GuildApiRoutes::removeMember(const std::string_view guildId,
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


std::expected<User, Error> GuildApiRoutes::modifyMember(const std::string_view guildId,
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
  User u(j["user"]);
  bot->user.cache.insert(u.id, u);
  u.guild = GuildUser(j);
  bot->guild.cache.insertGuildUser(std::string(guildId), u);
  return u;
}



std::expected<std::nullopt_t, Error> GuildApiRoutes::addMemberRole(const std::string_view guildId,
                                                                        const std::string_view userId,
                                                                        const std::string_view roleId,
                                                                        const std::optional<std::string>& auditLogReason)
{
  auto res = bot->rest.put(std::format(APIURL "/guilds/{}/members/{}/roles/{}", guildId, userId, roleId), std::nullopt,
    auditLogReason.has_value() ? 
      std::make_optional(std::vector<std::string>{"x-audit-log-reason: " + *auditLogReason }) : std::nullopt);
  if(!res.has_value() || res->second != 204){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to add role to guild member");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}


std::expected<std::nullopt_t, Error> GuildApiRoutes::removeMemberRole(const std::string_view guildId,
                                                                        const std::string_view userId,
                                                                        const std::string_view roleId,
                                                                        const std::optional<std::string>& auditLogReason)
{
  auto res = bot->rest.deletereq(std::format(APIURL "/guilds/{}/members/{}/roles/{}", guildId, userId, roleId),
    auditLogReason.has_value() ? 
      std::make_optional(std::vector<std::string>{"x-audit-log-reason: " + *auditLogReason }) : std::nullopt);
  if(!res.has_value() || res->second != 204){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to remove role from guild member");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
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
