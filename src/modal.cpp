#include "../include/modal.h"
#include <nlohmann/json.hpp>

Modal& Modal::setTitle(const std::string_view s){
  title = s;
  return *this;
}

Modal& Modal::setCustomId(const std::string_view s){
  customId = s;
  return *this;
}

nlohmann::json Modal::generate() const {
  nlohmann::json j;
  j["custom_id"] = customId;
  j["title"] = customId;
  j["components"] = nlohmann::json::array();
  for(const auto& a : components){
    j["components"].emplace_back(a->generate());
  }
  return j;
}
