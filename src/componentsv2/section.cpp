#include "../../include/componentsv2/section.h"

nlohmann::json SectionComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["components"] = nlohmann::json::array();
  for(auto& a : components){
    j["components"].emplace_back(a.generate());
  }
  j["accessory"] = accessory->generate();
  return j;
}
