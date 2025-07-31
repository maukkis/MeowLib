#include "../../include/componentsv2/mentionableSelect.h"


MentionableSelectComponent& MentionableSelectComponent::setCustomId(const std::string_view a){
  customId = a;
  return *this;
}


MentionableSelectComponent& MentionableSelectComponent::setMinValues(const int a){
  minValues = a;
  return *this;
}


MentionableSelectComponent& MentionableSelectComponent::setMaxValues(const int a){
  maxValues = a;
  return *this;
}


MentionableSelectComponent& MentionableSelectComponent::setPlaceHolder(const std::string_view a){
  placeholder = a;
  return *this;
}


nlohmann::json MentionableSelectComponent::generate(){
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
