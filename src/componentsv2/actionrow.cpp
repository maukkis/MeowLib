#include "../../include/componentsv2/actionRowComponent.h"
#include <utility>


template<>
nlohmann::json ActionRowComponent<true>::generate() {
  nlohmann::json j;
  j["type"] = type;
  j["components"] = nlohmann::json::array();
  for (const auto& a : components){
    j["components"].emplace_back(a->generate());
  }
  return j;
}

template<>
nlohmann::json ActionRowComponent<false>::generate() {
  static_assert(true, "no components in action row");
  std::unreachable();
}
