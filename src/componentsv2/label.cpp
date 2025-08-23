#include "../../include/componentsv2/label.h"
#include <nlohmann/json.hpp>

LabelComponent& LabelComponent::setLabel(const std::string_view s){
  label = s;
  return *this;
}


LabelComponent& LabelComponent::setDescription(const std::string_view s){
  description = s;
  return *this;
}

nlohmann::json LabelComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["label"] = label;
  if(!description.empty())
    j["description"] = description;
  j["component"] = component->generate();
  return j;
}
