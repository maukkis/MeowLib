#include "../../include/componentsv2/textInput.h"
#include <nlohmann/json_fwd.hpp>



nlohmann::json TextInputComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["style"] = static_cast<int>(style);
  j["custom_id"] = customId;
  if(maxLen)
    j["max_length"] = *maxLen;
  if(minLen)
    j["min_length"] = *minLen;
  j["required"] = required;
  if(!value.empty())
    j["value"] = value;
  if(!placeholder.empty())
    j["placeholder"] = placeholder;
  return j;
}



TextInputComponent&  TextInputComponent::setCustomId(const std::string_view s){
  customId = s;
  return *this;
}


TextInputComponent&  TextInputComponent::setStyle(const TextInputStyles s){
  style = s;
  return *this;
}


TextInputComponent&  TextInputComponent::setMinimumLength(const int i){
  minLen = i;
  return *this;
}


TextInputComponent&  TextInputComponent::setMaximumLength(const int i){
  maxLen = i;
  return *this;
}


TextInputComponent&  TextInputComponent::setRequired(const bool r){
  required = r;
  return *this;
}


TextInputComponent&  TextInputComponent::setValue(const std::string_view s){
  value = s;
  return *this;
}


TextInputComponent&  TextInputComponent::setPlaceholder(const std::string_view s){
  placeholder = s;
  return *this;
}
