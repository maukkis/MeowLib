#include "../include/invite.h"
#include "../include/helpers.h"
#include "../include/nyaBot.h"

Invite::Invite(const nlohmann::json& j){
  type = j["type"];
  code = j["code"];
  jsonToOptional(guild, j, "guild");
  jsonToOptional(channel, j, "channel");
  jsonToOptional(inviter, j, "inviter");
  jsonToOptional(targetType, j, "target_type");
  jsonToOptional(targetUser, j, "target_user");
  jsonToOptional(approximatePresenceCount, j, "approximate_presence_count");
  jsonToOptional(approximateMemberCount, j, "approximate_member_count");
  jsonToOptional(expiresAt, j, "expires_at");
  jsonToOptional(guildScheduledEvent, j, "guild_scheduled_event");
  jsonToOptional(guildScheduledEvent, j, "guild_scheduled_event");
  jsonToOptional(flags, j, "flags");
}


InviteCreateEvent::InviteCreateEvent(const nlohmann::json& j){
  code = j["code"];
  createdAt = j["created_at"];
  maxAge = j["max_age"];
  maxUses = j["max_uses"];
  temporary = j["temporary"];
  uses = j["uses"];
  jsonToOptional(guildId, j, "guild_id");
  channelId = j["channel_id"];
  jsonToOptional(inviter, j, "inviter");
  jsonToOptional(targetType, j, "target_type");
  jsonToOptional(targetUser, j, "target_user");
  jsonToOptional(expiresAt, j, "expires_at");
}


InviteDeleteEvent::InviteDeleteEvent(const nlohmann::json& j){
  code = j["code"];
  channelId = j["channel_id"];
  jsonToOptional(guildId, j, "guild_id");
}

InviteApiRoutes::InviteApiRoutes(NyaBot *bot) : bot{bot} {}


std::expected<Invite, Error> InviteApiRoutes::get(const std::string_view code, const bool withCounts){
  auto res = bot->rest.get(std::format(APIURL "/invites/{}?with_counts{}", code,
                                       withCounts ? "true" : "false"));
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to get invite");
    err.printErrors();
    return std::unexpected(err);
  }
  return Invite(nlohmann::json::parse(res->first));
}
std::expected<Invite, Error> InviteApiRoutes::remove(const std::string_view code){
  auto res = bot->rest.deletereq(std::format(APIURL "/invites/{}", code));
  if(!res.has_value() || res->second != 200){
    auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to delete invite");
    err.printErrors();
    return std::unexpected(err);
  }
  return Invite(nlohmann::json::parse(res->first));

}
