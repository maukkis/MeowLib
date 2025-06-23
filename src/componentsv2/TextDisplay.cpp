#include "../../include/componentsv2/textDisplay.h"


TextDisplayComponent& TextDisplayComponent::setContent(const std::string_view content){
  this->content = content;
  return *this;
}

nlohmann::json TextDisplayComponent::generate() {
  nlohmann::json j;
  j["type"] = type;
  j["content"] = content;
  return j;
}
