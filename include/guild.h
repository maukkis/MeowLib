#ifndef _INCLUDE_GUILD_H
#define _INCLUDE_GUILD_H
#include <optional>
#include <string>
#include "error.h"
#include <expected>
#include <nlohmann/json.hpp>
#include <string_view>
class NyaBot;

struct Guild {
  std::string id;
  std::string name;
  std::optional<std::string> icon = std::nullopt;
  std::optional<std::string> ownerId;
};

Guild deserializeGuild(const nlohmann::json& j);

class GuildApiRoutes {
public:
  GuildApiRoutes(NyaBot *bot);
  /// @brief fetches a guild by its id
  /// @param id guild id
  std::expected<Guild, Error> get(const std::string_view id);

  /// @brief fetches a guild preview by its id
  /// @param id guild id
  std::expected<Guild, Error> getPreview(const std::string_view id);
private:
  NyaBot* bot;
};

#endif
