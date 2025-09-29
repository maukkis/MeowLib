#ifndef _INCLUDE_GUILD_H
#define _INCLUDE_GUILD_H
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include "emoji.h"
#include "error.h"
#include <expected>
#include <string_view>
#include "role.h"
class NyaBot;


struct GuildBan {
  std::string guildId;
  User user;
};

GuildBan deserializeGuildBan(const nlohmann::json& j);

struct UnavailableGuild {
  std::string id;
  bool unavailable;
};

UnavailableGuild deserializeUnavailableGuild(const nlohmann::json& j);

struct GuildPreview{
  std::string id;
  std::string name;
  std::optional<std::string> icon;
  std::optional<std::string> splash;
  std::optional<std::string> discoverySplash;
  std::vector<Emoji> emojis;
  std::vector<std::string> features;
  int approximateMemberCount{};
  int approximatePresenceCount{};
  std::optional<std::string> description;
};


struct Guild {
  std::string id;
  std::string name;
  std::optional<std::string> icon = std::nullopt;
  std::optional<std::string> iconHash = std::nullopt;
  std::optional<std::string> splash = std::nullopt;
  std::optional<std::string> discoverySplash = std::nullopt;
  std::optional<bool> owner = std::nullopt;
  std::string ownerId;
  std::optional<std::string> permissions = std::nullopt;
  std::vector<Role> roles;
  std::vector<Emoji> emojis;
  std::optional<std::string> banner = std::nullopt;
  int nsfwLevel{};
};

Guild deserializeGuild(const nlohmann::json& j);
GuildPreview deserializeGuildPreview(const nlohmann::json& j);


class GuildApiRoutes {
public:
  GuildApiRoutes(NyaBot *bot);
  /// @brief fetches a guild by its id
  /// @param id guild id
  std::expected<Guild, Error> get(const std::string_view id);

  /// @brief fetches a guild preview by its id
  /// @param id guild id
  std::expected<GuildPreview, Error> getPreview(const std::string_view id);
private:
  NyaBot* bot;
};

#endif
