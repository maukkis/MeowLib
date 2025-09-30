#pragma once
#ifndef _INCLUDE_CHANNEL_H
#define _INCLUDE_CHANNEL_H
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

enum class ChannelType{
  GUILD_TEXT,
  DM,
  GUILD_VOICE,
  GROUP_DM,
  GUILD_CATEGORY,
  GUILD_ANNOUNCEMENT,
  ANNOUNCEMENT_THREAD = 10,
  PUBLIC_THREAD,
  PRIVATE_THREAD,
  GUILD_STAGE_VOICE,
  GUILD_DIRECTORY,
  GUILD_FORUM,
  GUILD_MEDIA,
};

struct Channel {
  std::string id{};
  ChannelType type{};
  std::optional<std::string> guildId;
  std::optional<int> position;
  std::optional<std::string> name;
  std::optional<std::string> topic;
};


Channel deserializeChannel(const nlohmann::json& j);

#endif 
