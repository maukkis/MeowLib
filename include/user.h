#pragma once
#ifndef _INCLUDE_USER_H
#define _INCLUDE_USER_H
#include "attachment.h"
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
  std::optional<std::string> nick = std::nullopt;
  std::optional<std::string> avatar = std::nullopt;
  std::optional<std::string> banner = std::nullopt;
  std::vector<std::string> roles;
  // only for guild member gw events
  std::optional<std::string> guildId = std::nullopt;
};

struct User {
  std::string id;
  std::string avatar;
  std::string discriminator;
  std::string globalName;
  std::string username;
  bool bot = false;
  std::optional<GuildUser> guild = std::nullopt;
  std::optional<PrimaryGuild> primaryGuild = std::nullopt;
};


class UserApiRoutes {
public:
  UserApiRoutes(NyaBot *bot);
  /// @brief fetches an user object
  /// @param id user id to fetch
  std::expected<User, Error> getUser(const std::string_view id);
  std::expected<User, Error> getCurrentUser();
  std::expected<Channel, Error> createDM(const std::string_view id);
  std::expected<User, Error> modifyCurrentUser(std::optional<std::string> username = std::nullopt,
                                               std::optional<Attachment> avatar = std::nullopt,
                                               std::optional<Attachment> banner = std::nullopt);
  std::expected<std::vector<Guild>, Error> getCurrentUserGuilds();
  std::expected<std::nullopt_t, Error> leaveGuild(const std::string_view id);

private:
  std::expected<std::string, Error> getReq(const std::string& endpoint);
  NyaBot *bot = nullptr;
};


GuildUser deserializeGuildUser(const nlohmann::json& j);
User deserializeUser(const nlohmann::json& j);
PrimaryGuild deserializePrimaryGuild(const nlohmann::json& j);
#endif
