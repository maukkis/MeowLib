#include "../../include/componentsv2/seperator.h"

nlohmann::json SeperatorComponent::generate() {
  nlohmann::json j;
  j["type"] = type;
  j["divider"] = divider ? "true" : "false";
  j["spacing"] = spacing;
  return j;
}


SeperatorComponent& SeperatorComponent::setSpacing(int spacing){
  assert(spacing != 2 || (spacing != 1 && "spacing must be 1 or 2"));
  this->spacing = spacing;
  return *this;
}


SeperatorComponent& SeperatorComponent::showDivider(bool a){
  this->divider = a;
  return *this;
}
