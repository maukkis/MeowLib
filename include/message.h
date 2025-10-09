#ifndef _INCLUDE_MESSAGE_H
#define _INCLUDE_MESSAGE_H
#include "componentsv2/componentsv2.h"
#include "attachment.h"
#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include "user.h"
#include <vector>
#include <memory>
#include "emoji.h"
#include <expected>
#include "error.h"


class NyaBot;


struct MessageDelete {
  std::string id;
  std::string channelId;
  std::optional<std::string> guildId;
};

enum class MessageReactionTypes {
  NORMAL,
  BURST
};

enum class MessageReferenceTypes {
  DEFAULT,
  FORWARD,
};

class MessageReference {
public:
  nlohmann::json generate() const;
  MessageReferenceTypes type{};
  std::optional<std::string> channelId = std::nullopt;
  std::optional<std::string> messageId = std::nullopt;
};


struct MessageReaction {
  MessageReaction(const nlohmann::json& j);
  std::string userId;
  std::string channelId;
  std::string messageId;
  std::optional<std::string> guildId = std::nullopt;
  std::optional<User> member = std::nullopt;
  Emoji emoji;
  std::optional<std::string> messageAuthorId = std::nullopt;
  bool burst;
  std::optional<std::vector<std::string>> burst_colors = std::nullopt;
  MessageReactionTypes type;
};


class Message {
public:
  nlohmann::json generate() const;
  Message() = default;
  Message(const nlohmann::json& j);
  Message(Message&&) = default;
  /// @brief sets the reply to the message if this was received as an event
  Message& replyTo();
  /// @brief sets the current message in this object to be forwarded
  Message& forward();
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
  std::string content;
  std::string id;
  int msgflags = 0;
  std::string timestamp;
  bool mentionEveryone;
  User author;
  std::optional<std::string> webhookId;
  std::string channelId;
  // ONLY FOR GATEWAY MESSAGE EVENTS
  std::optional<std::string> guildId;
  std::optional<MessageReference> messageReference;
private:
  std::vector<std::unique_ptr<Component>> components;
};

// TODO: create proper error parser and change the return values
class MessageApiRoutes {
public:
  MessageApiRoutes(NyaBot* bot);
  /// @brief creates a new message
  /// @param id channel id to send message in
  /// @param content message content
  std::expected<Message, Error> create(const std::string_view id, const std::string_view content);
  /// @brief creates a new message
  /// @param id channel id to send message in
  /// @param msg Message object
  std::expected<Message, Error> create(const std::string_view id, const Message& msg);

  std::expected<std::nullopt_t, Error> createReaction(const std::string_view channelId,
                                                      const std::string_view messageId,
                                                      const std::string_view emojiId);

  std::expected<std::nullopt_t, Error> removeOwnReaction(const std::string_view channelId,
                                                      const std::string_view messageId,
                                                      const std::string_view emojiId);
  std::expected<std::nullopt_t, Error> removeUsersReaction(const std::string_view channelId,
                                                           const std::string_view messageId,
                                                           const std::string_view emojiId,
                                                           const std::string_view userId);
private:
  std::expected<std::string, Error> send(const std::string_view id, const std::string& content);
  std::expected<std::string, Error> send(const std::string_view id, const std::string& content, const std::string& boundary);
  NyaBot *bot;
};

#endif

