#include "../../include/componentsv2/userSelect.h"


UserSelectComponent& UserSelectComponent::setCustomId(const std::string_view a){
  customId = a;
  return *this;
}


UserSelectComponent& UserSelectComponent::setMinValues(const int a){
  minValues = a;
  return *this;
}


UserSelectComponent& UserSelectComponent::setMaxValues(const int a){
  maxValues = a;
  return *this;
}


UserSelectComponent& UserSelectComponent::setPlaceHolder(const std::string_view a){
  placeholder = a;
  return *this;
}


nlohmann::json UserSelectComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["custom_id"] = customId;
  j["min_values"] = minValues;
  j["max_values"] = maxValues;
  if(!placeholder.empty())
    j["default_values"] = placeholder;
  j["options"] = nlohmann::json::array();
  for(auto& a : options){
    j["default_values"].push_back(a.serialize());
  }
  return j;
}
