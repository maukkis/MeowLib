#include "../include/message.h"
#include "../include/helpers.h"
#include <memory>
#include <optional>
#include <string_view>
#include <format>
#include "../include/nyaBot.h"

namespace {

InteractionMetaData deserializeInteractionMetaData(const nlohmann::json& j){
  return {
    .id = j["id"],
    .type = j["type"],
    .user = User(j["user"]),
    .targetUser = j.contains("target_user") ? std::make_optional(User(j["target_user"])) : std::nullopt
  };
}

}
nlohmann::json MessageReference::generate() const {
  nlohmann::json j;
  j["type"] = static_cast<int>(type);
  if(channelId)
    j["channel_id"] = *channelId;
  if(messageId)
    j["message_id"] = *messageId;
  return j;
}


MessageReaction::MessageReaction(const nlohmann::json& j){
  userId = j["user_id"];
  channelId = j["channel_id"];
  messageId = j["message_id"];
  if(j.contains("guild_id"))
    guildId = j["guild_id"];
  if(j.contains("member")){
    member = User(j["member"]["user"]);
    member->guild = GuildUser(j["member"]);
  }
  emoji = deserializeEmoji(j["emoji"]);
  if(j.contains("message_author_id"))
    messageAuthorId = j["message_author_id"];
  burst = j["burst"];
  if(burst)
    burst_colors = j["burst_colors"].get<std::vector<std::string>>();
  type = static_cast<MessageReactionTypes>(j["type"].get<int>());
}


Message::Message(const nlohmann::json& j){
  content = j["content"];
  msgflags = j["flags"];
  mentionEveryone = j["mention_everyone"];
  channelId = j["channel_id"];
  if(j.contains("interaction_metadata")){
    interactionData = deserializeInteractionMetaData(j["interaction_metadata"]);
  }
  if(j.contains("webhook_id"))
    webhookId = j["webhook_id"];
  author = User(j["author"]);
  if(j.contains("member"))
    author.guild = GuildUser(j["member"]);
  if(j.contains("guild_id"))
    guildId = j["guild_id"];
  id = j["id"];
  if(j.contains("poll"))
    poll = Poll(j["poll"]);
  if(j.contains("attachments")){
    for(const auto& a : j["attachments"]){
      resolvedAttachments.emplace_back(a);
    }
  }
  if(j.contains("referenced_message") && !j["referenced_message"].is_null()){
    referencedMessage = std::make_unique<Message>(j["referenced_message"]);
  }
}

Message& Message::addPoll(const Poll& a){
  poll = a;
  return *this;
}


Message& Message::replyTo(){
  if(!id.empty()){
    messageReference = MessageReference
      {
        .type = MessageReferenceTypes::DEFAULT,
        .messageId = id
      };
  }
  return *this;
}

Message& Message::forward(){
  if(!id.empty() && !channelId.empty()){
    messageReference = MessageReference
      {
        .type = MessageReferenceTypes::FORWARD,
        .channelId = channelId,
        .messageId = id
      };
  }
  return *this;
}

nlohmann::json Message::generate() const {
  nlohmann::json j;

  j["flags"] = msgflags;
  if(messageReference)
    j["message_reference"] = messageReference->generate();

  if(!content.empty()){
    j["content"] = content;
  }
  else if(!components.empty()){
    j["components"] = nlohmann::json::array();
    for(const auto& a : components){
      j["components"].emplace_back(a->generate());
    }
  }

  if(poll)
    j["poll"] = poll->generate();
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


std::expected<Message, Error> MessageApiRoutes::create(const std::string_view id, const std::string_view content){
  nlohmann::json j;
  j["content"] = content;

  return send(id, j.dump()).and_then([](std::expected<std::string, Error> a){
    return std::expected<Message, Error>(Message(nlohmann::json::parse(a.value())));
  });

}

std::expected<Message, Error> MessageApiRoutes::create(const std::string_view id, const Message& a){
  if(!a.attachments.empty()){
    auto payload = makeFormData(a.generate(), "woof", a.attachments);

    return send(id, payload, "woof").and_then([](std::expected<std::string, Error> a){
      return std::expected<Message, Error>(Message(nlohmann::json::parse(a.value())));
    });

  } else {
    return send(id, a.generate().dump()).and_then([](std::expected<std::string, Error> a){
      return std::expected<Message, Error>(Message(nlohmann::json::parse(a.value())));
    });

  }
}

std::expected<Message, Error> MessageApiRoutes::edit(const std::string_view channelId,
                                                     const std::string_view messageId,
                                                     const std::optional<const std::string_view> content,
                                                     const std::optional<const int> msgFlags)
{
  nlohmann::json j;
  if(content)
    j["content"] = *content;
  if(msgFlags)
    j["flags"] = *msgFlags;
  auto meow = bot->rest.patch(std::format(APIURL "/channels/{}/messages/{}", channelId, messageId),
                              j.dump());
  if(!meow.has_value() || meow->second != 200){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to edit a message");
    err.printErrors();
    return std::unexpected(err);
  }
  return Message(nlohmann::json::parse(meow->first));

}


std::expected<Message, Error> MessageApiRoutes::edit(const std::string_view channelId,
                                                     const std::string_view messageId,
                                                     const Message& a)
{
  
  std::expected<std::pair<std::string, int>, RestErrors> meow;
  if(a.attachments.empty()){
    meow = bot->rest.patch(std::format(APIURL "/channels/{}/messages/{}", channelId, messageId),
                           a.generate().dump());
  }
  else {
    auto payload = makeFormData(a.generate(), "woof", a.attachments);
    meow = bot->rest.sendFormData(std::format(APIURL "/channels/{}/messages/{}",
                                              channelId,
                                              messageId),
                                  payload,
                                  "woof",
                                  "PATCH");

  }
  if(!meow.has_value() || meow->second != 200){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to edit a message");
    err.printErrors();
    return std::unexpected(err);
  }
  return Message(nlohmann::json::parse(meow->first));

}

std::expected<std::nullopt_t, Error> MessageApiRoutes::remove(const std::string_view channelId,
                                                              const std::string_view messageId,
                                                              const std::optional<const std::string_view> reason)
{
  auto meow = bot->rest.deletereq(std::format(APIURL "/channels/{}/messages/{}", channelId, messageId),
                                  reason.has_value() ?
                                    std::make_optional(std::vector<std::string>{"x-audit-log-reason: " + std::string(*reason) })
                                    : std::nullopt);
  if(!meow.has_value() || meow->second != 204){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to delete a message");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}




std::expected<std::nullopt_t, Error> MessageApiRoutes::createReaction(const std::string_view channelId,
                                                                      const std::string_view messageId,
                                                                      const std::string_view emojiId)
{
  auto meow = bot->rest.put(std::format(APIURL "/channels/{}/messages/{}/reactions/{}/@me", channelId, messageId, asciiToURLEncoded(emojiId)));
  if(!meow.has_value() || meow->second != 204){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to create message reaction");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}


std::expected<std::nullopt_t, Error> MessageApiRoutes::removeOwnReaction(const std::string_view channelId,
                                                                      const std::string_view messageId,
                                                                      const std::string_view emojiId)
{
  auto meow = bot->rest.deletereq(std::format(APIURL "/channels/{}/messages/{}/reactions/{}/@me", channelId, messageId, asciiToURLEncoded(emojiId)));
  if(!meow.has_value() || meow->second != 204){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to remove own message reaction");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}

std::expected<std::nullopt_t, Error> MessageApiRoutes::removeUsersReaction(const std::string_view channelId,
                                                                           const std::string_view messageId,
                                                                           const std::string_view emojiId,
                                                                           const std::string_view userId)
{
  auto meow = bot->rest.deletereq(std::format(APIURL "/channels/{}/messages/{}/reactions/{}/{}", channelId, messageId, asciiToURLEncoded(emojiId), userId));
  if(!meow.has_value() || meow->second != 204){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to remove user message reaction");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;

}

std::expected<std::string, Error> MessageApiRoutes::send(const std::string_view id, const std::string& content){
  auto meow = bot->rest.post(std::format(APIURL "/channels/{}/messages", id),
                             content);
  if(!meow.has_value() || meow->second != 200){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to create a message");
    err.printErrors();
    return std::unexpected(err);
  }
  return meow->first;
}

std::expected<std::string, Error> MessageApiRoutes::send(const std::string_view id, const std::string& content, const std::string& boundary){
  auto meow = bot->rest.sendFormData(std::format(APIURL "/channels/{}/messages", id),
                             content,
                             boundary,
                             "POST");
  if(!meow.has_value() || meow->second != 200){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to create a message");
    err.printErrors();
    return std::unexpected(err);
  }
  return meow->first;
}
