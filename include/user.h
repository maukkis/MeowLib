#pragma once
#include <cstdint>
#ifndef _INCLUDE_USER_H
#define _INCLUDE_USER_H
#include "attachment.h"
#include "cache.h"
#include "channel.h"
#include "error.h"
#include <expected>
#include <optional>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
class NyaBot;
struct Guild;

struct PrimaryGuild {
  std::string identityGuildId;
  std::optional<bool> identityEnabled;
  std::string tag;
  std::string badge;
};




struct GuildUser {
  GuildUser() = default;
  GuildUser(const nlohmann::json& j);
  std::optional<std::string> nick = std::nullopt;
  std::optional<std::string> avatar = std::nullopt;
  std::optional<std::string> banner = std::nullopt;
  std::vector<std::string> roles = {};
  std::optional<uint64_t> permissions = std::nullopt;
  std::optional<std::string> channelId = std::nullopt;
  // only for guild member gw events
  std::optional<std::string> guildId = std::nullopt;
  // an iso8601 timestamp 
  std::optional<std::string> communicationDisabledUntil = std::nullopt;
};

struct User {
  User() = default;
  User(const nlohmann::json& j);
  std::string id;
  std::string avatar;
  std::string discriminator;
  std::string globalName;
  std::string username;
  bool bot = false;
  std::optional<GuildUser> guild = std::nullopt;
  std::optional<PrimaryGuild> primaryGuild = std::nullopt;
};



class UserCache : public GenericDiscordCache<User> {
public:
  using GenericDiscordCache::GenericDiscordCache;

  ~UserCache(){
    Log::dbg("cache hits: " + std::to_string(hits) + " misses: " + std::to_string(misses));
  }


  void setCurrentUser(const User& u);
  std::expected<User, Error> getCurrentUser();
private:
  std::optional<User> currentUser;
};





class UserApiRoutes {
public:
  UserApiRoutes(NyaBot *bot);
  /// @brief fetches an user object
  /// @param id user id to fetch
  std::expected<User, Error> get(const std::string_view id, const bool force = false);
  std::expected<User, Error> getCurrent();
  std::expected<Channel, Error> createDM(const std::string_view id);
  std::expected<User, Error> modifyCurrentUser(std::optional<std::string> username = std::nullopt,
                                               std::optional<Attachment> avatar = std::nullopt,
                                               std::optional<Attachment> banner = std::nullopt);
  std::expected<std::vector<Guild>, Error> getCurrentUserGuilds();
  std::expected<std::nullopt_t, Error> leaveGuild(const std::string_view id);
  UserCache cache;
private:
  std::expected<std::string, Error> getReq(const std::string& endpoint);
  NyaBot *bot = nullptr;
};


nlohmann::json serializeGuildUser(const GuildUser& a);
PrimaryGuild deserializePrimaryGuild(const nlohmann::json& j);
#endif
