#ifndef _INCLUDE_PRESENCE_H
#define _INCLUDE_PRESENCE_H
#include <optional>
#include <string>
#include <nlohmann/json.hpp>

enum class ActivityTypes {
  Playing = 0,
  Streaming,
  Listening,
  Watching,
  Custom,
  Competing,
};

struct Activity{
  std::string name;
  ActivityTypes type;
  std::string state = "";
  std::optional<std::string> url = std::nullopt;
};

struct Presence {
  std::optional<int> since = std::nullopt;
  std::optional<Activity> activity = std::nullopt;
  std::string status;
  bool afk = false;
};

nlohmann::json serializeActivity(const Activity& a);
nlohmann::json serializePresence(const Presence& p);
#endif
