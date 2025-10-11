#ifndef _INCLUDE_AUTOMOD_H
#define _INCLUDE_AUTOMOD_H
#include "error.h"
#include <expected>
#include <string>
#include <nlohmann/json_fwd.hpp>
#include <vector>

class NyaBot;

enum class AutoModEventType {
  MESSAGE_SEND = 1,
  MESSAGE_UPDATE = 2
};

enum class AutoModTriggerType {
  KEYWORD = 1,
  SPAM,
  KEYWORD_PRESET,
  MENTION_SPAM,
  MEMBER_PROFILE
};

enum class AutoModKeywordPresetTypes {
  PROFANITY = 1,
  SEXUAL_CONTENT,
  SLURS
};

enum class AutoModActionType {
  BLOCK_MESSAGE = 1,
  SEND_ALERT_MESSAGE,
  TIMEOUT,
  BLOCK_MEMBER_INTERACTION
};


struct AutoModAction{
  AutoModAction() = default;
  AutoModAction(const nlohmann::json& j);
  nlohmann::json generate() const;
  AutoModActionType type;

  template<AutoModActionType i>
  auto getMetadata(){
    return std::get<static_cast<int>(i)-1>(metadata);
  }

  template<AutoModActionType i, typename T>
  void setMetadata(T&& a){
    metadata.emplace<static_cast<int>(i)-1>(std::forward<T>(a));
  }
  std::variant<std::string, std::string, int> metadata;
};


struct AutoModTriggerMetadata {
  AutoModTriggerMetadata() = default;
  nlohmann::json generate(AutoModTriggerType a) const;
  AutoModTriggerMetadata(const nlohmann::json& j, AutoModTriggerType a);
  std::vector<std::string> keywordFilter;
  std::vector<std::string> regexPatterns;
  std::vector<AutoModKeywordPresetTypes> presets;
  std::vector<std::string> allowList;
  int mentionTotalLimit{};
  bool mentionRaidProtectionEnabled{};
};

struct AutoModRule {
  AutoModRule() = default;
  AutoModRule(const nlohmann::json& j);
  nlohmann::json generate() const;
  std::string id;
  std::string guildId;
  std::string name;
  std::string creatorId;
  AutoModEventType eventType;
  AutoModTriggerType triggerType;
  AutoModTriggerMetadata triggerMetadata;
  std::vector<AutoModAction> actions;
  bool enabled = false;
  std::vector<std::string> exemptRoles;
  std::vector<std::string> exemptChannels;
};



struct AutoModActionExecution {
  AutoModActionExecution(const nlohmann::json& j);
  std::string guildId;
  AutoModAction action;
  std::string ruleId;
  AutoModTriggerType type;
  std::string userId;
  std::optional<std::string> channelId;
  std::optional<std::string> messageId;
  std::optional<std::string> alertSystemMessageId;
  std::string content;
  std::optional<std::string> matchedKeyword;
  std::string matchedContent;
};


class AutoModApiRoutes {
public:
  AutoModApiRoutes(NyaBot *b);
  std::expected<std::vector<AutoModRule>, Error> listAutoModerationRules(const std::string_view guildId);
  std::expected<AutoModRule, Error> getAutoModerationRule(const std::string_view guildId,
                                                          const std::string_view ruleId);
  std::expected<AutoModRule, Error> createAutoModerationRule(const std::string_view guildId,
                                                             const AutoModRule& r);
  std::expected<AutoModRule, Error> modifyAutoModerationRule(const std::string_view guildId,
                                                             const std::string_view ruleId,
                                                             const AutoModRule& r);
  std::expected<std::nullopt_t, Error> deleteAutoModerationRule(const std::string_view guildId,
                                                                const std::string_view ruleId);
private:
  std::expected<std::string, Error> getReq(const std::string& endpoint);
  NyaBot *bot;
};

#endif
