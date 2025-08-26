#include "../../include/componentsv2/stringSelect.h"


StringSelectComponent& StringSelectComponent::setCustomId(const std::string_view a){
  customId = a;
  return *this;
}


StringSelectComponent& StringSelectComponent::setMinValues(const int a){
  minValues = a;
  return *this;
}


StringSelectComponent& StringSelectComponent::setMaxValues(const int a){
  maxValues = a;
  return *this;
}


StringSelectComponent& StringSelectComponent::setPlaceHolder(const std::string_view a){
  placeholder = a;
  return *this;
}

StringSelectComponent& StringSelectComponent::setRequired(const bool a){
  required = a;
  return *this;
}

nlohmann::json StringSelectOption::serialize(){
  nlohmann::json j;
  j["label"] = label;
  j["value"] = value;
  if(description)
    j["description"] = *description;
  return j;
}


nlohmann::json StringSelectComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["custom_id"] = customId;
  j["min_values"] = minValues;
  j["max_values"] = maxValues;
  if(required)
    j["required"] = *required;
  if(!placeholder.empty())
    j["placeholder"] = placeholder;
  j["options"] = nlohmann::json::array();
  for(auto& a : options){
    j["options"].push_back(a.serialize());
  }
  return j;
}
