#include "../include/componentsv2.h"
#include <nlohmann/json.hpp>
/* dont mind this is from testing previously
nlohmann::json Media::generate(){
  nlohmann::json j;
  j["url"] = url;
  return j;
}


nlohmann::json Component::generate(){
  nlohmann::json j;
  if(!components.empty()){
    j["components"] = nlohmann::json::array();
    for(const auto& a : components)
      j["components"].push_back(a->generate());
  }
  j["type"] = type;
  if(!customId.empty())
    j["custom_id"] = customId;
  if(accentColor)
    j["accent_color"] = *accentColor;
  if(spoiler)
   j["spoiler"] = *spoiler;
  if(spacing)
    j["spacing"] = *spacing;
  if(!content.empty()){
    if(type != BUTTON)
      j["content"] = content;
    else
      j["label"] = content;
  }
  if(!label.empty())
    j["label"] = label;
  if(media){
    j["items"] = nlohmann::json::array();
    j["items"].push_back(media->generate());
  }
  if(style)
    j["style"] = *style;
  return j;
}


Component& Component::addComponent(Component a){
  components.push_back(std::make_shared<Component>(std::move(a)));
  return *this;
}


Component& Component::setType(ComponentTypes b){
  type = b;
  return *this;
}


Component& Component::setId(const std::string_view id){
  customId = id;
  return *this;
}


Component& Component::setAccentColor(uint32_t color){
  accentColor = color;
  return *this;
}

Component& Component::setSpoiler(bool a){
  spoiler = a;
  return *this;
}


Component& Component::setSpacing(int a){
  spacing = a;
  return *this;
}


Component& Component::setContent(const std::string_view a){
  content = a;
  return *this;
}


Component& Component::setLabel(const std::string_view b){
  label = b;
  return *this;
}

Component& Component::setStyle(int style){
  this->style = style;
  return *this;
}*/
