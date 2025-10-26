#ifndef _INCLUDE_GUILD_H
#define _INCLUDE_GUILD_H
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include "emoji.h"
#include "error.h"
#include <expected>
#include <string_view>
#include <vector>
#include "role.h"
#include <coroutine>

class NyaBot;


struct GuildMemberRequestTask {
  std::vector<User> users = {};
  std::coroutine_handle<> hp;
  bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<> handle) noexcept {
    hp = handle;
  }
  std::vector<User> await_resume() const noexcept {
    return users;
  }
};


struct GuildBan {
  std::optional<std::string> guildId = std::nullopt;
  std::optional<std::string> reason = std::nullopt;
  User user;
};

GuildBan deserializeGuildBan(const nlohmann::json& j);

struct UnavailableGuild {
  std::string id;
  bool unavailable;
};

UnavailableGuild deserializeUnavailableGuild(const nlohmann::json& j);

struct GuildPreview{
  std::string id;
  std::string name;
  std::optional<std::string> icon;
  std::optional<std::string> splash;
  std::optional<std::string> discoverySplash;
  std::vector<Emoji> emojis;
  std::vector<std::string> features;
  int approximateMemberCount{};
  int approximatePresenceCount{};
  std::optional<std::string> description;
};


struct Guild {
  std::string id;
  std::string name;
  std::optional<std::string> icon = std::nullopt;
  std::optional<std::string> iconHash = std::nullopt;
  std::optional<std::string> splash = std::nullopt;
  std::optional<std::string> discoverySplash = std::nullopt;
  std::optional<bool> owner = std::nullopt;
  std::string ownerId;
  std::optional<std::string> permissions = std::nullopt;
  std::vector<Role> roles;
  std::vector<Emoji> emojis;
  std::optional<std::string> banner = std::nullopt;
  int nsfwLevel{};
};

Guild deserializeGuild(const nlohmann::json& j);
GuildPreview deserializeGuildPreview(const nlohmann::json& j);


class GuildApiRoutes {
public:
  GuildApiRoutes(NyaBot *bot);
  /// @brief fetches a guild by its id
  /// @param id guild id
  std::expected<Guild, Error> get(const std::string_view id);

  /// @brief fetches a guild preview by its id
  /// @param id guild id
  std::expected<GuildPreview, Error> getPreview(const std::string_view id);
  
  std::expected<std::vector<Channel>, Error> getChannels(const std::string_view guildId);

  std::expected<Channel, Error> createChannel(const std::string_view guildId, const Channel& a);


  std::expected<std::vector<GuildBan>, Error> getBans(const std::string_view id);
  
  std::expected<GuildBan, Error> getBan(const std::string_view guildId, const std::string_view userId);
  
  std::expected<std::nullopt_t, Error> createBan(const std::string_view guildId,
                                                      const std::string_view userId,
                                                      const std::optional<int> deleteMessagesSeconds = std::nullopt,
                                                      const std::optional<std::string>& auditLogReason = std::nullopt);
  
  std::expected<std::nullopt_t, Error> removeBan(const std::string_view guildId, const std::string_view userId,
                                                      const std::optional<std::string>& auditLogReason = std::nullopt);

  std::expected<std::nullopt_t, Error> removeMember(const std::string_view guildId, const std::string_view userId,
                                                         const std::optional<std::string>& auditLogReason = std::nullopt);

  std::expected<User, Error> modifyMember(const std::string_view guildId,
                                               const std::string_view userId,
                                               const GuildUser& user,
                                               const std::optional<std::string>& reason);

  std::expected<std::nullopt_t, Error> addMemberRole(const std::string_view guildId,
                                                          const std::string_view userId,
                                                          const std::string_view roleId,
                                                          const std::optional<std::string>& auditLogReason = std::nullopt);


  std::expected<std::nullopt_t, Error> removeMemberRole(const std::string_view guildId,
                                                             const std::string_view userId,
                                                             const std::string_view roleId,
                                                             const std::optional<std::string>& auditLogReason = std::nullopt);



private:
  std::expected<std::string, Error> getReq(const std::string& endpoint);
  NyaBot* bot;
};

#endif
