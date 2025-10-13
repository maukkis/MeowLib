#pragma once
#ifndef _INCLUDE_CHANNEL_H
#define _INCLUDE_CHANNEL_H
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <expected>
#include <string_view>
#include "error.h"

class NyaBot;

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
  std::optional<std::string> guildId = std::nullopt;
  std::optional<int> position = std::nullopt;
  std::optional<std::string> name = std::nullopt;
  std::optional<std::string> topic = std::nullopt;
};


class ChannelApiRoutes {
public:
  ChannelApiRoutes(NyaBot *bot);
  
  /// @brief modifies a channel
  /// @param id channel id
  /// @param ch Channel object
  std::expected<Channel, Error> modify(const std::string_view id, const Channel& ch);
  /// @brief deletes/closes a channel
  /// @param id channel id
  // stupid C++ not letting me have a function called delete grrrr *bites*
  std::expected<Channel, Error> close(const std::string_view id);
private:
  NyaBot *bot;
};


Channel deserializeChannel(const nlohmann::json& j);
nlohmann::json serializeChannel(const Channel& a);
#endif 
