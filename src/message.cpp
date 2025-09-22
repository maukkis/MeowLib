#include "../include/message.h"
#include "../include/helpers.h"
#include <optional>
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


std::expected<std::nullopt_t, Error> MessageApiRoutes::create(const std::string_view id, const std::string_view content){
  nlohmann::json j;
  j["content"] = content;
  return send(id, j.dump()); 
}

std::expected<std::nullopt_t, Error> MessageApiRoutes::create(const std::string_view id, const Message& a){
  if(!a.attachments.empty()){
    auto payload = makeFormData(a.generate(), "woof", a.attachments);
    return send(id, payload, "woof");
  } else {
    return send(id, a.generate().dump());
  }
}

std::expected<std::nullopt_t, Error> MessageApiRoutes::send(const std::string_view id, const std::string& content){
  auto meow = bot->rest.post(std::format(APIURL "/channels/{}/messages", id),
                             content);
  if(!meow.has_value() || meow->second != 200){
    Log::error("failed to create a message\n" + meow.value_or(std::make_pair("", 0)).first);
    return std::unexpected(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return std::nullopt;
}

std::expected<std::nullopt_t, Error> MessageApiRoutes::send(const std::string_view id, const std::string& content, const std::string& boundary){
  auto meow = bot->rest.sendFormData(std::format(APIURL "/channels/{}/messages", id),
                             content,
                             boundary,
                             "POST");
  if(!meow.has_value() || meow->second != 200){
    Log::error("failed to create a message\n" + meow.value_or(std::make_pair("", 0)).first);
    return std::unexpected(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return std::nullopt;
}
