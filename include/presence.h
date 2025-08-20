#ifndef _INCLUDE_PRESENCE_H
#define _INCLUDE_PRESENCE_H
#include <optional>
#include <string>
#include <nlohmann/json.hpp>

struct Activities{
  std::string name;
  int type; // make enum later;
  std::string state;
  std::optional<std::string> url;
};

struct Presence {
  std::optional<int> since;
  std::optional<Activities> activities = std::nullopt;
  std::string status;
  bool afk = false;
};

nlohmann::json serializeActivity(const Activities& a);
nlohmann::json serializePresence(const Presence& p);
#endif
