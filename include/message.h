#ifndef _INCLUDE_MESSAGE_H
#define _INCLUDE_MESSAGE_H
#include "componentsv2/componentsv2.h"
#include "attachment.h"
#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <memory>
class NyaBot;


class Message {
public:
  nlohmann::json generate() const;
  Message() = default;
  Message(Message&&) = default;
  template<typename T>
  requires(std::is_base_of_v<Component, std::remove_reference_t<T>>)
  Message& addComponent(T&& comp){
    components.emplace_back(std::make_unique<std::remove_cvref_t<T>>(std::forward<T>(comp)));
    return *this;
  }
  Message& addAttachment(const Attachment& a);
  Message& setMessageFlags(int flags);
  Message& setContent(const std::string_view a);
  std::vector<Attachment> attachments;
private:
  std::string content;
  int msgflags = 0;
  std::vector<std::unique_ptr<Component>> components;
};

// TODO: create proper error parser and change the return values
class MessageApiRoutes {
public:
  MessageApiRoutes(NyaBot* bot);
  /// @brief creates a new message
  /// @param id channel id to send message in
  /// @param content message content
  void create(const std::string_view id, const std::string_view content);
  /// @brief creates a new message
  /// @param id channel id to send message in
  /// @param msg Message object
  void create(const std::string_view id, const Message& msg);
private:
  void send(const std::string_view id, const std::string& content);
  void send(const std::string_view id, const std::string& content, const std::string& boundary);
  NyaBot *bot;
};

#endif

