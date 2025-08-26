#include "../include/emoji.h"

nlohmann::json serializeEmoji(const Emoji& e){
  nlohmann::json j;
  j["name"] = e.name;
  j["id"] = e.id;
  if(e.animated)
    j["animated"] = *e.animated;
  return j;
}
