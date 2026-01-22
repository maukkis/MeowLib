#include "../../include/componentsv2/checkboxGroupAction.h"
#include <nlohmann/json.hpp>

nlohmann::json CheckboxGroupActionComponent::generate() {
  nlohmann::json j;
  j["type"] = type;
  j["custom_id"] = customId;
  if(minValues)
    j["min_values"] = *minValues;
  if(maxValues)
    j["max_values"] = *maxValues;
  if(required)
    j["required"] = *required;
  for(const auto& a : options){
    j["options"].emplace_back(a.generate());
  }
  return j;
}

CheckboxGroupActionComponent& CheckboxGroupActionComponent::setCustomId(const std::string_view a){
  customId = a;
  return *this;
}


CheckboxGroupActionComponent& CheckboxGroupActionComponent::setMinValues(const int a){
  minValues = a;
  return *this;
}


CheckboxGroupActionComponent& CheckboxGroupActionComponent::setMaxValues(const int a){
  maxValues = a;
  return *this;
}


CheckboxGroupActionComponent& CheckboxGroupActionComponent::setRequired(const bool a){
  required = a;
  return *this;
}
