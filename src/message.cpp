#include "../include/message.h"
#include "../include/helpers.h"
#include <string_view>
#include "../include/nyaBot.h"





nlohmann::json Message::generate() const {
  nlohmann::json j;
  if(!content.empty()){
    j["content"] = content;
    if(msgflags != 0) j["flags"] = msgflags;
  }
  if(!components.empty()){
    j["flags"] = 1 << 15 | msgflags;
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

Message& Message::setMessageFlags(int msgflags){
  this->msgflags = msgflags;
  return *this;
}

MessageApiRoutes::MessageApiRoutes(NyaBot *bot) : bot{bot}{}


void MessageApiRoutes::create(const std::string_view id, const std::string_view content){
  nlohmann::json j;
  j["content"] = content;
  send(id, j.dump()); 
}

void MessageApiRoutes::create(const std::string_view id, const Message& a){
  if(!a.attachments.empty()){
    auto payload = makeFormData(a.generate(), "woof", a.attachments);
    send(id, payload, "woof");
  } else {
    send(id, a.generate().dump());
  }
}

void MessageApiRoutes::send(const std::string_view id, const std::string& content){
  auto meow = bot->rest.post(std::format("https://discord.com/api/v10/channels/{}/messages", id),
                             content);
  if(!meow.has_value() || meow->second != 200){
    Log::error("failed to create a message\n" + meow.value_or(std::make_pair("", 0)).first);
  }
}

void MessageApiRoutes::send(const std::string_view id, const std::string& content, const std::string& boundary){
  auto meow = bot->rest.sendFormData(std::format("https://discord.com/api/v10/channels/{}/messages", id),
                             content,
                             boundary,
                             "POST");
  if(!meow.has_value() || meow->second != 200){
    Log::error("failed to create a message\n" + meow.value_or(std::make_pair("", 0)).first);
  }
}
