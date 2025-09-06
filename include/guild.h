#ifndef _INCLUDE_GUILD_H
#define _INCLUDE_GUILD_H
#include <optional>
#include <string>
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
  Guild get(const std::string_view id);
  Guild getPreview(const std::string_view id);
private:
  NyaBot* bot;
};

#endif
