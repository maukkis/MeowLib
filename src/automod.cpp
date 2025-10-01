#include "../include/automod.h"
#include "../include/nyaBot.h"
#include <nlohmann/json.hpp>


AutoModAction::AutoModAction(const nlohmann::json& j){
  type = j["type"];
  if(j["metadata"].empty()) return;
  switch(type){ 
    case AutoModActionType::BLOCK_MESSAGE:
      metadata.emplace<static_cast<int>(AutoModActionType::BLOCK_MESSAGE)-1>(j["metadata"]["custom_message"]);
    break;
    case AutoModActionType::TIMEOUT:
      metadata.emplace<static_cast<int>(AutoModActionType::TIMEOUT)-1>(j["metadata"]["timeout"]);
    break;
    case AutoModActionType::SEND_ALERT_MESSAGE:
      metadata.emplace<static_cast<int>(AutoModActionType::SEND_ALERT_MESSAGE)-1>(j["metadata"]["channel_id"]);
    default: break;
  }

}


nlohmann::json AutoModAction::generate() const {
  nlohmann::json j;
  j["type"] = type;
  switch(type){ 
    case AutoModActionType::BLOCK_MESSAGE:
      j["metadata"]["custom_message"] = std::get<0>(metadata);
    break;
    case AutoModActionType::TIMEOUT:
      j["metadata"]["timeout"] = std::get<2>(metadata);
    break;
    case AutoModActionType::SEND_ALERT_MESSAGE:
      j["metadata"]["channel_id"] = std::get<1>(metadata);
    default: break;
  }
  return j;
}



AutoModTriggerMetadata::AutoModTriggerMetadata(const nlohmann::json& j, AutoModTriggerType type){
  if(type == AutoModTriggerType::KEYWORD || type == AutoModTriggerType::MEMBER_PROFILE){
    if(j.contains("keyword_filter"))
      keywordFilter = j["keyword_filter"].get<std::vector<std::string>>();
    if(j.contains("regex_patterns"))
      regexPatterns = j["regex_patterns"].get<std::vector<std::string>>();
  }
  if(type == AutoModTriggerType::KEYWORD_PRESET){
    if(j.contains("presets")){
      for(const auto& a : j["presets"])
        presets.emplace_back(static_cast<AutoModKeywordPresetTypes>(a));
    }
  }
  switch(type){
    case AutoModTriggerType::KEYWORD:
    case AutoModTriggerType::KEYWORD_PRESET:
    case AutoModTriggerType::MEMBER_PROFILE:
      if(j.contains("allow_list"))
        allowList = j["allow_list"].get<std::vector<std::string>>();
    break;
    default: break;
  }
  if(type == AutoModTriggerType::MENTION_SPAM){
    if(j.contains("mention_total_limit"))
      mentionTotalLimit = j["mention_total_limit"];
    if(j.contains("mention_raid_protection_enabled"))
      mentionRaidProtectionEnabled = j["mention_raid_protection_enabled"];
  }
}


nlohmann::json AutoModTriggerMetadata::generate(AutoModTriggerType type) const {
  nlohmann::json j;
  if(type == AutoModTriggerType::KEYWORD || type == AutoModTriggerType::MEMBER_PROFILE){
    j["keyword_filter"] = keywordFilter;
    j["regex_patterns"] = regexPatterns;
  }
  if(type == AutoModTriggerType::KEYWORD_PRESET)
    j["presets"] = presets;
  switch(type){
    case AutoModTriggerType::KEYWORD:
    case AutoModTriggerType::KEYWORD_PRESET:
    case AutoModTriggerType::MEMBER_PROFILE:
      j["allow_list"] = allowList;
    break;
    default: break;
  }
  if(type == AutoModTriggerType::MENTION_SPAM){
    j["mention_total_limit"] = mentionTotalLimit;
    j["mention_raid_protection_enabled"] = mentionRaidProtectionEnabled;
  }
  return j;
}



AutoModRule::AutoModRule(const nlohmann::json& j) {
  id  =  j["id"];
  guildId = j["guild_id"];
  name = j["name"];
  creatorId = j["creator_id"];
  eventType = static_cast<AutoModEventType>(j["event_type"]);
  triggerType = static_cast<AutoModTriggerType>(j["trigger_type"]);
  triggerMetadata = AutoModTriggerMetadata(j["trigger_metadata"], triggerType);
  enabled = j["enabled"];
  exemptRoles = j["exempt_roles"].get<std::vector<std::string>>();
  exemptChannels = j["exempt_channels"].get<std::vector<std::string>>();

  for(const auto& a : j["actions"])
    actions.emplace_back(a);
}

nlohmann::json AutoModRule::generate() const{
  nlohmann::json j;
  j["name"] = name;
  j["event_type"] = eventType;
  j["trigger_type"] = triggerType;
  if(triggerType != AutoModTriggerType::SPAM)
    j["trigger_metadata"] = triggerMetadata.generate(triggerType);
  j["actions"] = nlohmann::json::array();
  for(const auto& a : actions){
    j["actions"].push_back(a.generate());
  }
  j["enabled"] = enabled;
  if(!exemptRoles.empty())
    j["exempt_roles"] = exemptRoles;
  if(!exemptChannels.empty())
    j["exempt_channels"] = exemptChannels;
  return j;
}


AutoModApiRoutes::AutoModApiRoutes(NyaBot *bot): bot{bot}{}


std::expected<std::vector<AutoModRule>, Error> AutoModApiRoutes::listAutoModerationRules(const std::string_view guildId){
  return getReq(std::format(APIURL "/guilds/{}/auto-moderation/rules", guildId))
    .and_then([](std::expected<std::string, Error> a){
      std::vector<AutoModRule> b;
      nlohmann::json j = nlohmann::json::parse(a.value());
      for(const auto& c : j)
        b.emplace_back(AutoModRule(c));
      return std::expected<std::vector<AutoModRule>, Error>(std::move(b));
    });
}

std::expected<AutoModRule, Error> AutoModApiRoutes::getAutoModerationRule(const std::string_view guildId,
                                                                          const std::string_view ruleId)
{
  return getReq(std::format(APIURL "/guilds/{}/auto-moderation/rules/{}", guildId, ruleId))
    .and_then([](std::expected<std::string, Error> a){
      return std::expected<AutoModRule, Error>(AutoModRule(nlohmann::json::parse(a.value())));
    });
}

std::expected<AutoModRule, Error> AutoModApiRoutes::createAutoModerationRule(const std::string_view guildId,
                                                                             const AutoModRule& r)
{
  auto res = bot->rest.post(std::format(APIURL "/guilds/{}/auto-moderation/rules", guildId), r.generate().dump());
  if(!res.has_value() || res->second != 200){
    Log::error("failed to create automod rule "
             + res.value_or(std::make_pair("", 0)).first);
    return std::unexpected(Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first));
  }
  return AutoModRule(nlohmann::json::parse(res->first));
}

std::expected<std::string, Error> AutoModApiRoutes::getReq(const std::string& endpoint){
  auto res = bot->rest.get(endpoint);
  if(!res.has_value() || res->second != 200){
    Log::error("failed to fetch automod rule(s) "
             + res.value_or(std::make_pair("", 0)).first);
    return std::unexpected(Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first));
  }
  return res->first;
}
