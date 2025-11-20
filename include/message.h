#ifndef _INCLUDE_MESSAGE_H
#define _INCLUDE_MESSAGE_H
#include "componentsv2/componentsv2.h"
#include "attachment.h"
#include <expected>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include "user.h"
#include "poll.h"
#include <vector>
#include <memory>
#include "emoji.h"
#include <expected>
#include "error.h"

enum MsgFlags {
  SUPPRESS_EMBEDS = 1 << 2,
  EPHEMERAL = 1 << 6,
  SUPPRESS_NOTIFICATIONS = 1 << 12,
  IS_COMPONENTS_V2 = 1 << 15,
};



struct InteractionData{
  std::string id;
  int type;
  std::string name;
  User user;
};

struct SelectComponent;
class LabelComponent;
class ButtonComponent;
class TextInputComponent;
class FileUploadComponent;

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



// we should actually check if it is a top level component
template<typename T, typename = void>
struct TopLevelComponent : std::true_type {};

template<typename T>
struct TopLevelComponent<T, std::enable_if_t<std::is_base_of_v<SelectComponent, T>, void>> : std::false_type {};

template<>
struct TopLevelComponent<ButtonComponent> : std::false_type {};

template<>
struct TopLevelComponent<LabelComponent> : std::false_type {};

template<>
struct TopLevelComponent<TextInputComponent> : std::false_type {};

template<>
struct TopLevelComponent<FileUploadComponent> : std::false_type {};

template<typename T>
concept topLevelComponent = TopLevelComponent<std::remove_cvref_t<T>>::value;


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
  template<topLevelComponent T>
  Message& addComponent(T&& comp){
    msgflags |= MsgFlags::IS_COMPONENTS_V2;
    components.emplace_back(std::make_unique<std::remove_cvref_t<T>>(std::forward<T>(comp)));
    return *this;
  }
  Message& addPoll(const Poll& a);
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
  std::optional<InteractionData> interactionData;
  std::optional<Poll> poll;
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

  /// @brief edits a message
  /// @param channelId channel id where the message is
  /// @param messageId message id to edit
  /// @param content optionally message content
  /// @param msgFlags optionally message flags
  std::expected<Message, Error> edit(const std::string_view channelId,
                                     const std::string_view messageId,
                                     const std::optional<const std::string_view> content = std::nullopt,
                                     const std::optional<const int> msgFlags = std::nullopt);
  /// @brief edits a message
  /// @param channelId channel id where the message is
  /// @param messageId message id to edit
  /// @param msg Message object
  std::expected<Message, Error> edit(const std::string_view channelId,
                                     const std::string_view messageId,
                                     const Message& msg);

  std::expected<std::nullopt_t, Error> remove(const std::string_view channelId,
                                              const std::string_view messageId,
                                              const std::optional<const std::string_view> reason = std::nullopt);


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

