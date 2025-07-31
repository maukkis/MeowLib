#include "../include/message.h"

nlohmann::json Message::generate() const {
  nlohmann::json j;
  if(!content.empty()){
    j["content"] = content;
  }
  if(!components.empty()){
    j["flags"] = 1 << 15;
    j["components"] = nlohmann::json::array();
    for(const auto& a : components){
      j["components"].emplace_back(a->generate());
    }
  }
  return j;
}

Message& Message::addAttachment(const Attachment& a){
  attachments.push_back(std::move(a));
  return *this;
}

Message& Message::setContent(const std::string_view a){
  content = a;
  return *this;
}
