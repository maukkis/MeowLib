#include "../../include/componentsv2/buttonComponent.h"

ButtonComponent& ButtonComponent::setLabel(const std::string_view a){
  label = a;
  return *this;
}

ButtonComponent& ButtonComponent::setStyle(enum Style s){
  style = s;
  return *this;
}

ButtonComponent& ButtonComponent::setCustomId(const std::string_view a){
  customId = a;
  return *this;
}
