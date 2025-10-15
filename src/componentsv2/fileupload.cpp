#include "../../include/componentsv2/fileupload.h"


nlohmann::json FileUploadComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["custom_id"] = customId;
  j["min_values"] = minValues;
  j["max_values"] = maxValues;
  j["required"] = required;
  return j;
}


FileUploadComponent& FileUploadComponent::setCustomId(const std::string_view id){
  customId = id;
  return *this;
}


FileUploadComponent& FileUploadComponent::setMinValues(int values){
  minValues = values;
  return *this;
}

FileUploadComponent& FileUploadComponent::setMaxValues(int values){
  maxValues = values;
  return *this;
}

FileUploadComponent& FileUploadComponent::setRequired(bool required){
  this->required = required;
  return *this;
}
