#ifndef _INCLUDE_USER_H
#define _INCLUDE_USER_H
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
  User getUser(const std::string_view id);
private:
  NyaBot *bot = nullptr;
};


GuildUser deserializeGuildUser(nlohmann::json& j);
User deserializeUser(nlohmann::json& j);
#endif
