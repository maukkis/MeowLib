#include "../../include/componentsv2/textDisplay.h"


TextDisplay& TextDisplay::setContent(const std::string_view content){
  this->content = content;
  return *this;
}

nlohmann::json TextDisplay::generate() {
  nlohmann::json j;
  j["type"] = type;
  j["content"] = content;
  return j;
}
