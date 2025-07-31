#include "../../include/componentsv2/channelSelect.h"


ChannelSelectComponent& ChannelSelectComponent::setCustomId(const std::string_view a){
  customId = a;
  return *this;
}


ChannelSelectComponent& ChannelSelectComponent::setMinValues(const int a){
  minValues = a;
  return *this;
}


ChannelSelectComponent& ChannelSelectComponent::setMaxValues(const int a){
  maxValues = a;
  return *this;
}


ChannelSelectComponent& ChannelSelectComponent::setPlaceHolder(const std::string_view a){
  placeholder = a;
  return *this;
}


nlohmann::json ChannelSelectComponent::generate(){
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
