#include "../../include/componentsv2/buttonComponent.h"



nlohmann::json ButtonComponent::generate() {
  nlohmann::json j;
  j["type"] = type;
  if(label) j["label"] = *label;
  j["style"] = style;
  j["custom_id"] = customId;
  return j;
}



ButtonComponent& ButtonComponent::setLabel(const std::string_view a) {
  label = a;
  return *this;
}

ButtonComponent& ButtonComponent::setStyle(enum Style s) {
  style = s;
  return *this;
}

ButtonComponent& ButtonComponent::setCustomId(const std::string_view a) {
  customId = a;
  return *this;
}
