#pragma once
#include "cache.h"
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


struct ThreadMetadata {
  ThreadMetadata() = default;
  ThreadMetadata(const nlohmann::json& j);
  bool archived{};
  int autoArchiveDuration{};
  std::string archiveTimestamp;
  bool locked{};
  std::optional<bool> invitable;
  std::optional<std::string> createTimestamp;
};


struct Channel {
  Channel() = default;
  Channel(const nlohmann::json& j);
  nlohmann::json generate() const;
  std::string id{};
  ChannelType type{};
  std::optional<std::string> guildId = std::nullopt;
  std::optional<int> position = std::nullopt;
  std::optional<std::string> name = std::nullopt;
  std::optional<std::string> topic = std::nullopt;
  std::optional<bool> nsfw = std::nullopt;
  std::optional<std::string> lastPinTimestamp = std::nullopt;
  std::optional<std::string> parentId = std::nullopt;
  std::optional<std::string> lastMessageId = std::nullopt;
  // only for gateway thread events
  std::optional<bool> newlyCreated = std::nullopt;
  std::optional<ThreadMetadata> threadMetadata = std::nullopt;
};


class ChannelCache : public GenericDiscordCache<Channel> {
public:
  using GenericDiscordCache::GenericDiscordCache;
  ~ChannelCache(){
    Log::dbg("cache hits: " + std::to_string(hits) + " misses: " + std::to_string(misses));
  }
};



class ChannelApiRoutes {
public:
  ChannelApiRoutes(NyaBot *bot);
 
  std::expected<Channel, Error> get(const std::string_view id, const bool force = false);
  /// @brief modifies a channel
  /// @param id channel id
  /// @param ch Channel object
  std::expected<Channel, Error> modify(const std::string_view id, const Channel& ch);
  /// @brief deletes/closes a channel
  /// @param id channel id
  // stupid C++ not letting me have a function called delete grrrr *bites*
  std::expected<Channel, Error> close(const std::string_view id);
  /// @brief joins a thread
  /// @param id id of the thread
  std::expected<std::nullopt_t, Error> joinThread(const std::string_view id);
  /// @brief triggers the typing indicator for 10 seconds
  /// @param id channel id
  std::expected<std::nullopt_t, Error> triggerTypingIndicator(const std::string_view id);
  ChannelCache cache;
private:
  NyaBot *bot;
};


#endif 
