#ifndef _INCLUDE_EMOJI_H
#define _INCLUDE_EMOJI_H
#include <string>
#include <optional>
#include <nlohmann/json.hpp>


struct Emoji {
  std::string id;
  std::string name;
  std::optional<bool> animated = std::nullopt;
};

nlohmann::json serializeEmoji(const Emoji& e);

#endif
