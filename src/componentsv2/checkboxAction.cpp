#include "../../include/componentsv2/checkboxAction.h"
#include <nlohmann/json.hpp>

nlohmann::json CheckboxActionComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["custom_id"] = customId;
  if(defaultt)
    j["default"] = *defaultt;
  return j;
}


CheckboxActionComponent& CheckboxActionComponent::setCustomId(const std::string_view a){
  customId = a;
  return *this;
}


CheckboxActionComponent& CheckboxActionComponent::setDefault(const bool a){
  defaultt = a;
  return *this;
}
