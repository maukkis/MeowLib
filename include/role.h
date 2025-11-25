#ifndef _INCLUDE_ROLE_H
#define _INCLUDE_ROLE_H
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

struct RoleColors{
  RoleColors();
  RoleColors(const nlohmann::json& j);
  int primaryColor{};
  std::optional<int> secondaryColor;
  std::optional<int> tertiaryColor;
};


struct Role {
  Role() = default;
  Role(const nlohmann::json& j);
  std::string id;
  std::string name;
  RoleColors colours;
  bool hoist;
  std::optional<std::string> icon;
  std::optional<std::string> unicodeEmoji;
  int position;
  std::string permissions;
  bool managed;
  bool mentionable;
  int flags;
};


struct RoleEvent {
  std::string guildId;
  Role role;
};

struct RoleDeleteEvent {
  std::string guildId;
  std::string roleId;
};

#endif
