#ifndef _INCLUDE_USER_H
#define _INCLUDE_USER_H
#include "channel.h"
#include "error.h"
#include <expected>
#include <optional>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
class NyaBot;


struct GuildUser {
  std::string nick;
  std::string avatar;
  std::string banner;
  std::vector<std::string> roles;
};

struct User {
  std::string id;
  std::string avatar;
  std::string discriminator;
  std::string globalName;
  std::string username;
  std::optional<GuildUser> guild = std::nullopt;
};


class UserApiRoutes {
public:
  UserApiRoutes(NyaBot *bot);
  /// @brief fetches an user object
  /// @param id user id to fetch
  std::expected<User, Error> getUser(const std::string_view id);
  std::expected<Channel, Error> createDM(const std::string_view id);
private:
  NyaBot *bot = nullptr;
};


GuildUser deserializeGuildUser(const nlohmann::json& j);
User deserializeUser(const nlohmann::json& j);
#endif
