#include "../include/message.h"

nlohmann::json Message::generate(){
  nlohmann::json j;
  if(!components.empty()){
    j["flags"] = 1 << 15;
    j["components"] = nlohmann::json::array();
    for(const auto& a : components){
      auto cj = a->generate();
      j["components"].emplace_back(cj);
    }
  }
  return j;
}
