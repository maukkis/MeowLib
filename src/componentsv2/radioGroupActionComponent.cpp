#include "../../include/componentsv2/radioGroupAction.h"


nlohmann::json RadioGroupOption::generate() const {
  nlohmann::json j;
  j["value"] = value;
  j["label"] = label;
  if(desc)
    j["desc"] = *desc;
  if(defaultt)
    j["default"] = *defaultt;
  return j;
}


RadioGroupOption& RadioGroupOption::setValue(const std::string_view a){
  value = a;
  return *this;
}


RadioGroupOption& RadioGroupOption::setLabel(const std::string_view a){
  label = a;
  return *this;
}


RadioGroupOption& RadioGroupOption::setDescription(const std::string_view a){
  desc = a;
  return *this;
}


RadioGroupOption& RadioGroupOption::setDefault(const bool a){
  defaultt = a;
  return *this;
}

nlohmann::json RadioGroupActionComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["custom_id"] = customId;
  j["required"] = required;
  for(const auto& a : options){
    j["options"].emplace_back(a.generate());
  }
  return j;
}

RadioGroupActionComponent& RadioGroupActionComponent::setCustomId(const std::string_view a){
  customId = a;
  return *this;
}

RadioGroupActionComponent& RadioGroupActionComponent::setRequired(const bool a){
  required = a;
  return *this;
}
