#include "../../include/componentsv2/container.h"


nlohmann::json ContainerComponent::generate() {
  nlohmann::json j;
  j["type"] = type;
  j["components"] = nlohmann::json::array();
  for(const auto& a : components){
    j["components"].emplace_back(a->generate());
  }
  if(accentColor) j["accent_color"] = *accentColor;
  j["spoiler"] = spoiler ? "true" : "false";
  return j;
}


ContainerComponent& ContainerComponent::setAccentColor(int color) {
  accentColor = std::make_optional(color);
  return *this; 
}


ContainerComponent& ContainerComponent::setSpoiler(bool val) {
  spoiler = val;
  return *this;
}
