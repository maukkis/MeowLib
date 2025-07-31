#include "../../include/componentsv2/file.h"

nlohmann::json FileComponent::generate(){
  nlohmann::json j;
  j["type"] = type;
  j["file"]["url"] = fileName;
  j["spoiler"] = spoiler;
  return j;
}

FileComponent& FileComponent::setFile(const std::string_view s){
  fileName = s;
  return *this;
}

FileComponent& FileComponent::setSpoiler(bool s){
  spoiler = s;
  return *this;
}
