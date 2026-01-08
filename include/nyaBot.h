#ifndef nyaBot_H
#define nyaBot_H
#include <condition_variable>
#include <meowHttp/enum.h>
#include <meowHttp/websocket.h>
#include "async.h"
#include "buttonInteraction.h"
#include "channel.h"
#include "contextMenuCommand.h"
#include "emoji.h"
#include "guild.h"
#include "shard.h"
#include "guildScheduledEvent.h"
#include "invite.h"
#include "message.h"
#include "modalInteraction.h"
#include "contextMenuInteraction.h"
#include "poll.h"
#include "restclient.h"
#include "automod.h"
#include "role.h"
#include "selectInteraction.h"
#include "slashCommandInt.h"
#include "presence.h"
#include "slashCommands.h"
#include <expected>
#include <functional>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include "error.h"
#include <string>
#include <atomic>
#include "log.h"
#include <nlohmann/json.hpp>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include "commandHandling.h"
#include "thread.h"
#include "typingstart.h"
#include "user.h"
#include "voice.h"



struct ImportantApiStuff {
  std::string token;
  std::mutex UnavailableGuildIdsmtx;
  std::unordered_set<std::string> unavailableGuildIds;
  std::string appId;
  std::string defaultUrl;
  int intents = 0;
  int numShards = -1;
};

struct Ready {
  Ready(const nlohmann::json& j);
  int v{};
  User user;
  std::vector<UnavailableGuild> guilds;
  int shardId{};
  //this should be 1 because number of shards cannot be below 1
  int numShards = 1;
};


struct Funs {
  std::function<void(AutoModRule&)> onAutoModerationRuleCreateF = {};
  std::function<void(AutoModRule&)> onAutoModerationRuleUpdateF = {};
  std::function<void(AutoModRule&)> onAutoModerationRuleDeleteF = {};
  std::function<void(AutoModActionExecution&)> onAutoModerationActionExecutionF = {};


  std::function<void(SlashCommandInteraction&)> onSlashF = {};
  std::function<void(Ready&)> onReadyF = {};
  std::function<void()> onAutocompleteF = {};
  std::function<void(ButtonInteraction&)> onButtonF = {};
  std::function<void(SelectInteraction&)> onSelectF = {};
  std::function<void(ModalInteraction&)> onModalF = {};
  std::function<void(ContextMenuInteraction&)> onContextMenuF = {};


  std::function<void(Message&)> onMessageCreateF = {};
  std::function<void(Message&)> onMessageUpdateF = {};
  std::function<void(MessageDelete&)> onMessageDeleteF = {};
  std::function<void(MessageReaction&)> onMessageReactionAddF = {};
  std::function<void(MessageReaction&)> onMessageReactionRemoveF = {};

  std::function<void(MessagePollVote&)> onMessagePollVoteAddF = {};
  std::function<void(MessagePollVote&)> onMessagePollVoteRemoveF = {};


  std::function<void(Guild&)> onGuildCreateF = {};
  std::function<void(Guild&)> onGuildUpdateF = {};
  std::function<void(UnavailableGuild&)> onGuildDeleteF = {};
  std::function<void(GuildBan&)> onGuildBanAddF = {};
  std::function<void(GuildBan&)> onGuildBanRemoveF = {};
  std::function<void(User&)> onGuildMemberRemoveF = {};
  std::function<void(User&)> onGuildMemberAddF = {};
  std::function<void(User&)> onGuildMemberUpdateF = {};
  std::function<void(RoleEvent&)> onGuildRoleCreateF = {};
  std::function<void(RoleEvent&)> onGuildRoleUpdateF = {};
  std::function<void(RoleDeleteEvent&)> onGuildRoleDeleteF = {};
  
  std::function<void(GuildScheduledEvent&)> onGuildScheduledEventCreateF = {};
  std::function<void(GuildScheduledEvent&)> onGuildScheduledEventUpdateF = {};
  std::function<void(GuildScheduledEvent&)> onGuildScheduledEventDeleteF = {};

  std::function<void(GuildScheduledEventUser&)> onGuildScheduledEventUserAddF = {};
  std::function<void(GuildScheduledEventUser&)> onGuildScheduledEventUserRemoveF = {};

  std::function<void(Channel&)> onChannelCreateF = {};
  std::function<void(Channel&)> onChannelUpdateF = {};
  std::function<void(Channel&)> onChannelDeleteF = {};

  std::function<void(Channel&)> onThreadCreateF = {};
  std::function<void(Channel&)> onThreadUpdateF = {};
  std::function<void(Channel&)> onThreadDeleteF = {};
  std::function<void(ThreadListSync&)> onThreadListSyncF = {};
  std::function<void(ThreadMember&)> onThreadMemberUpdateF = {};
  std::function<void(ThreadMembersUpdate&)> onThreadMembersUpdateF = {}; 
  std::function<void(InviteCreateEvent&)> onInviteCreateF = {};
  std::function<void(InviteDeleteEvent&)> onInviteDeleteF = {};

  std::function<void(TypingStart&)> onTypingStartF = {};

  std::function<void(User&)> onUserUpdateF = {};

  std::function<void(VoiceState&)> onVoiceStateUpdateF = {};
};


// this function has a few edgecases where it doesnt work but its fine <3
template<typename F, typename... Args>
void runOnce(F&& f, Args&&... a) {
  [[maybe_unused]]
  static bool ran = [&](){
    std::invoke(std::forward<F>(f), std::forward<Args>(a)...);
    return true;
  }();
} 



struct InteractionCallbacks {
  std::unordered_map<std::string, std::function<void(ButtonInteraction&)>> buttonInteractionTable;
  std::unordered_map<std::string, std::function<void(SelectInteraction&)>> selectInteractionTable;
  std::unordered_map<std::string, std::function<void(ModalInteraction&)>> modalInteractionTable;
  std::unordered_map<std::string, std::function<void(ContextMenuInteraction&)>> contextMenuInteractionTable;
};

namespace Intents{
enum Intents {
  GUILDS = 1 << 0,
  GUILD_MEMBERS = 1 << 1,
  GUILD_MODERATION = 1 << 2,
  GUILD_EXPRESSIONS = 1 << 3,
  GUILD_INTEGRATIONS = 1 << 4,
  GUILD_WEBHOOKS =  1 << 5,
  GUILD_INVITES = 1 << 6,
  GUILD_VOICE_STATES = 1 << 7,
  GUILD_PRESENCES = 1 << 8,
  GUILD_MESSAGES = 1 << 9,
  GUILD_MESSAGE_REACTIONS = 1 << 10,
  GUILD_MESSAGE_TYPING = 1 << 11,
  DIRECT_MESSAGES = 1 << 12,
  DIRECT_MESSAGE_REACTIONS = 1 << 13,
  DIRECT_MESSAGE_TYPING = 1 << 14,
  MESSAGE_CONTENT = 1 << 15,
  GUILD_SCHEDULED_EVENTS = 1 << 16,
  AUTO_MODERATION_CONFIGURATION = 1 << 20,
  AUTO_MODERATION_EXECUTION = 1 << 21,
  GUILD_MESSAGE_POLLS = 1 << 24,
  DIRECT_MESSAGE_POLLS = 1 << 25
};
}

template<typename T>
concept HasOnCommandImplemented = !std::is_same_v<decltype(&Command::onCommand), decltype(&T::onCommand)>;

template<typename T>
concept CommandHandler = std::is_base_of_v<Command, T> && HasOnCommandImplemented<T>;



class NyaBot {
public:
  NyaBot(int intents = 0);
  ~NyaBot();
  ///
  /// @brief Starts the bot.
  /// @param token the bot token
  ///
  void run(const std::string_view token);
  ///
  /// @brief closes the gateway connection effectively stopping the bot
  ///
  void close();
  ///
  /// @brief Registers a slash command with the bot.
  /// @param slash SlashCommand object
  ///
  void addSlash(SlashCommand& slash);
  void addSlash(SlashCommand&& slash);
  void removeSlash(const std::string_view name);
  void addContextMenuCommand(const ContextMenuCommand& cmd);
  ///
  /// @brief Registers a command handler with the library.
  /// @param commandName commands custom_id to handle
  /// @param p unique_ptr to the command object
  ///
  void addCommandHandler(const std::string& commandName, std::unique_ptr<Command> p){
    commands[commandName] = std::move(p);
  }


  template<CommandHandler Command, typename... Args>
  void addCommandHandler(const std::string& commandName, Args&&... args){
    commands[commandName] = std::make_unique<Command>(std::forward<Args>(args)...);
  }

  void removeCommandHandler(const std::string& name){
    if(commands.contains(name))
      commands.erase(name);
  }
  
  void forceShardCount(int count);

  void enableDebugLogging(){
    Log::enabled = true;
  }
  
  ///@brief sets the amount of times to retry a failed request due to an I/O error
  ///@param amount amount to set
  void setRetryAmount(int amount){
    retryAmount = amount;
  }
  
  ///
  /// @brief gets the current ping with the gateway in ms
  /// @returns integer that has the ping in ms
  int getPing(int shardId = 0);

  ///
  /// @brief gets the current gateway state
  ///
  GatewayStates getCurrentState(int shardId = 0);
  ///
  /// @brief gets the number of shards
  ///
  int getNumShards();
  ///
  /// @brief changes the presence of the bot user the gateway connection must be on ready state for this to be called
  /// @param p presence object
  ///
  void changePresence(const Presence& p, std::optional<std::string> guildId = std::nullopt);

  ///
  /// @brief sets the presence to be used when identifying
  /// @param p presence object
  ///
  void setPresence(const Presence& p);

  void onAutoModerationRuleCreate(std::function<void(AutoModRule&)> f);
  void onAutoModerationRuleUpdate(std::function<void(AutoModRule&)> f);
  void onAutoModerationRuleDelete(std::function<void(AutoModRule&)> f);
  void onAutoModerationActionExecution(std::function<void(AutoModActionExecution&)> f);

  void onMessagePollVoteRemove(std::function<void(MessagePollVote&)> f);
  void onMessagePollVoteAdd(std::function<void(MessagePollVote&)> f);


  void onReady(std::function<void()> f);
  void onReady(std::function<void(Ready&)> f);
  void onSlash(std::function<void(SlashCommandInteraction&)> f);
  void onAutocomplete(std::function<void()> f);
  void onButtonPress(std::function<void(ButtonInteraction&)> f);
  void onSelectInteraction(std::function<void(SelectInteraction&)> f);
  void onModalSubmit(std::function<void(ModalInteraction&)> f);

  void onMessageCreate(std::function<void(Message&)> f);
  void onMessageUpdate(std::function<void(Message&)> f);
  void onMessageDelete(std::function<void(MessageDelete&)> f);
  void onMessageReactionAdd(std::function<void(MessageReaction&)> f);
  void onMessageReactionRemove(std::function<void(MessageReaction&)> f);

  void onGuildCreate(std::function<void(Guild&)> f);
  void onGuildUpdate(std::function<void(Guild&)> f);
  void onGuildDelete(std::function<void(UnavailableGuild&)> f);
  void onGuildBanAdd(std::function<void(GuildBan&)> f);
  void onGuildBanRemove(std::function<void(GuildBan&)> f);
  void onGuildMemberRemove(std::function<void(User&)> f);
  void onGuildMemberAdd(std::function<void(User&)> f);
  void onGuildMemberUpdate(std::function<void(User&)> f);
  void onGuildRoleCreate(std::function<void(RoleEvent&)> f);
  void onGuildRoleUpdate(std::function<void(RoleEvent&)> f);
  void onGuildRoleDelete(std::function<void(RoleDeleteEvent&)> f);
  
  void onGuildScheduledEventCreate(std::function<void(GuildScheduledEvent&)> f);
  void onGuildScheduledEventUpdate(std::function<void(GuildScheduledEvent&)> f);
  void onGuildScheduledEventDelete(std::function<void(GuildScheduledEvent&)> f);

  void onGuildScheduledEventUserAdd(std::function<void(GuildScheduledEventUser&)> f);
  void onGuildScheduledEventUserRemove(std::function<void(GuildScheduledEventUser&)> f);

  void onChannelCreate(std::function<void(Channel&)> f);
  void onChannelUpdate(std::function<void(Channel&)> f);
  void onChannelDelete(std::function<void(Channel&)> f);

  void onThreadCreate(std::function<void(Channel&)> f);
  void onThreadUpdate(std::function<void(Channel&)> f);
  void onThreadDelete(std::function<void(Channel&)> f);
  void onThreadListSync(std::function<void(ThreadListSync&)> f);
  void onThreadMemberUpdate(std::function<void(ThreadMember&)> f);
  void onThreadMembersUpdate(std::function<void(ThreadMembersUpdate&)> f);

  void onInviteCreate(std::function<void(InviteCreateEvent&)> f);
  void onInviteDelete(std::function<void(InviteDeleteEvent&)> f);

  void onTypingStart(std::function<void(TypingStart&)> f);

  void onUserUpdate(std::function<void(User&)> f);

  void onVoiceStateUpdate(std::function<void(VoiceState&)> f);

  void onContextMenuCommand(std::function<void(ContextMenuInteraction&)> f);
  ///
  /// @brief Adds a callback when a certain interaction happens.
  /// @param s interaction's custom_id
  ///
  void addInteractionCallback(const std::string_view s, std::function<void(ButtonInteraction&)> f);
  ///
  /// @brief Adds a callback when a certain interaction happens.
  /// @param s interaction's custom_id
  ///
  void addInteractionCallback(const std::string_view s, std::function<void(SelectInteraction&)> f);
  
  void addInteractionCallback(const std::string_view s, std::function<void(ModalInteraction&)> f);

  void addInteractionCallback(const std::string_view s, std::function<void(ContextMenuInteraction&)> f);
  [[deprecated("use syncApplicationCommands")]]
  std::expected<std::nullopt_t, Error> syncSlashCommands();

  std::expected<std::nullopt_t, Error> syncApplicationCommands();
  /// @brief request guild members from a guild from the gw
  /// @param guildId guild to request from
  /// @param query what the username has to start with leave as "" for all
  /// @param limit max amount of members to receive leave 0 for no limit
  /// @returns MeowAsync with a vector of users you can use .get() to get the vector
  MeowAsync<std::vector<User>> requestGuildMembers(const std::string_view guildId,
                                                   const std::string_view query,
                                                   const int limit);
  RestClient rest{this};
  UserApiRoutes user{this};
  GuildApiRoutes guild{this};
  EmojiApiRoutes emoji{this};
  MessageApiRoutes message{this};
  AutoModApiRoutes automod{this};
  ChannelApiRoutes channel{this};
  InviteApiRoutes invite{this};
  VoiceApiRoutes voice{this};
private:

  void routeInteraction(ButtonInteraction& interaction);
  void routeInteraction(SelectInteraction& interaction);
  void routeInteraction(ModalInteraction& interaction);
  void routeInteraction(ContextMenuInteraction& interaction);


  void autoModerationRuleCreate(nlohmann::json j);
  void autoModerationRuleUpdate(nlohmann::json j);
  void autoModerationRuleDelete(nlohmann::json j);
  void autoModerationActionExecution(nlohmann::json j);

  void ready(nlohmann::json j);
  void interaction(nlohmann::json j);
  void resumed(nlohmann::json j);

  void messageCreate(nlohmann::json j);
  void messageUpdate(nlohmann::json j);
  void messageDelete(nlohmann::json j);
  void messageReactionAdd(nlohmann::json j);
  void messageReactionRemove(nlohmann::json j);

  void messagePollVoteAdd(nlohmann::json j);
  void messagePollVoteRemove(nlohmann::json j);

  void guildCreate(nlohmann::json j);
  void guildUpdate(nlohmann::json j);
  void guildDelete(nlohmann::json j);
  void guildBanAdd(nlohmann::json j);
  void guildBanRemove(nlohmann::json j);
  void guildMemberRemove(nlohmann::json j);
  void guildMemberAdd(nlohmann::json j);
  void guildMemberUpdate(nlohmann::json j);
  void guildMemberChunk(nlohmann::json j);
  void guildRoleCreate(nlohmann::json j);
  void guildRoleUpdate(nlohmann::json j);
  void guildRoleDelete(nlohmann::json j);
  
  void guildSchedCreate(nlohmann::json j);
  void guildSchedUpdate(nlohmann::json j);
  void guildSchedDelete(nlohmann::json j);

  void guildSchedUserAdd(nlohmann::json j);
  void guildSchedUserRemove(nlohmann::json j);


  void channelCreate(nlohmann::json j);
  void channelUpdate(nlohmann::json j);
  void channelDelete(nlohmann::json j);

  void threadCreate(nlohmann::json j);
  void threadUpdate(nlohmann::json j);
  void threadDelete(nlohmann::json j);
  void threadListSync(nlohmann::json j);
  void threadMemberUpdate(nlohmann::json j);
  void threadMembersUpdate(nlohmann::json j);
  void inviteCreate(nlohmann::json j);
  void inviteDelete(nlohmann::json j);

  void typingStart(nlohmann::json j);

  void rateLimited(nlohmann::json j);
  
  void userUpdate(nlohmann::json j);

  void voiceServerUpdate(nlohmann::json j);
  void voiceStateUpdate(nlohmann::json j);

  // these are undocumented and are just placeholders
  void voiceChannelStatusUpdate(nlohmann::json j);
  void voiceChannelStartTimeUpdate(nlohmann::json j);

  static void signalHandler(int){
    a->stop = true;
  }

  void globalSend(const std::string& payload);
  inline static NyaBot *a = nullptr;
  std::atomic<bool> stop;
  meowWs::Websocket handle;
 
  std::unordered_map<std::string_view, std::function<void(nlohmann::json)>> dispatchHandlers {
    {"AUTO_MODERATION_RULE_CREATE", std::bind(&NyaBot::autoModerationRuleCreate, this, std::placeholders::_1)},
    {"AUTO_MODERATION_RULE_UPDATE", std::bind(&NyaBot::autoModerationRuleUpdate, this, std::placeholders::_1)},
    {"AUTO_MODERATION_RULE_DELETE", std::bind(&NyaBot::autoModerationRuleDelete, this, std::placeholders::_1)},
    {"AUTO_MODERATION_ACTION_EXECUTION", std::bind(&NyaBot::autoModerationActionExecution, this, std::placeholders::_1)},
    {"INTERACTION_CREATE", std::bind(&NyaBot::interaction, this, std::placeholders::_1)},
    {"READY", std::bind(&NyaBot::ready, this, std::placeholders::_1)},
    {"RESUMED", std::bind(&NyaBot::resumed, this, std::placeholders::_1)},
    {"MESSAGE_CREATE", std::bind(&NyaBot::messageCreate, this, std::placeholders::_1)},
    {"MESSAGE_UPDATE", std::bind(&NyaBot::messageUpdate, this, std::placeholders::_1)},
    {"MESSAGE_DELETE", std::bind(&NyaBot::messageDelete, this, std::placeholders::_1)},
    {"MESSAGE_POLL_VOTE_ADD", std::bind(&NyaBot::messagePollVoteAdd, this, std::placeholders::_1)},
    {"MESSAGE_POLL_VOTE_REMOVE", std::bind(&NyaBot::messagePollVoteRemove, this, std::placeholders::_1)},
    {"GUILD_CREATE", std::bind(&NyaBot::guildCreate, this, std::placeholders::_1)},
    {"GUILD_UPDATE", std::bind(&NyaBot::guildUpdate, this, std::placeholders::_1)},
    {"GUILD_DELETE", std::bind(&NyaBot::guildDelete, this, std::placeholders::_1)},
    {"GUILD_BAN_ADD", std::bind(&NyaBot::guildBanAdd, this, std::placeholders::_1)},
    {"GUILD_BAN_REMOVE", std::bind(&NyaBot::guildBanRemove, this, std::placeholders::_1)},
    {"GUILD_MEMBER_REMOVE", std::bind(&NyaBot::guildMemberRemove, this, std::placeholders::_1)},
    {"GUILD_MEMBER_ADD", std::bind(&NyaBot::guildMemberAdd, this, std::placeholders::_1)},
    {"GUILD_MEMBER_UPDATE", std::bind(&NyaBot::guildMemberUpdate, this, std::placeholders::_1)},
    {"GUILD_MEMBERS_CHUNK", std::bind(&NyaBot::guildMemberChunk, this, std::placeholders::_1)},
    {"GUILD_ROLE_CREATE", std::bind(&NyaBot::guildRoleCreate, this, std::placeholders::_1)},
    {"GUILD_ROLE_UPDATE", std::bind(&NyaBot::guildRoleUpdate, this, std::placeholders::_1)},
    {"GUILD_ROLE_DELETE", std::bind(&NyaBot::guildRoleDelete, this, std::placeholders::_1)},
    {"GUILD_SCHEDULED_EVENT_CREATE", std::bind(&NyaBot::guildSchedCreate, this, std::placeholders::_1)},
    {"GUILD_SCHEDULED_EVENT_UPDATE", std::bind(&NyaBot::guildSchedUpdate, this, std::placeholders::_1)},
    {"GUILD_SCHEDULED_EVENT_DELETE", std::bind(&NyaBot::guildSchedDelete, this, std::placeholders::_1)},
    {"GUILD_SCHEDULED_EVENT_USER_ADD", std::bind(&NyaBot::guildSchedUserAdd, this, std::placeholders::_1)},
    {"GUILD_SCHEDULED_EVENT_USER_REMOVE", std::bind(&NyaBot::guildSchedUserRemove, this, std::placeholders::_1)},
    {"MESSAGE_REACTION_ADD", std::bind(&NyaBot::messageReactionAdd, this, std::placeholders::_1)},
    {"MESSAGE_REACTION_REMOVE", std::bind(&NyaBot::messageReactionRemove, this, std::placeholders::_1)},
    {"CHANNEL_CREATE", std::bind(&NyaBot::channelCreate, this, std::placeholders::_1)},
    {"CHANNEL_UPDATE", std::bind(&NyaBot::channelUpdate, this, std::placeholders::_1)},
    {"CHANNEL_DELETE", std::bind(&NyaBot::channelDelete, this, std::placeholders::_1)},
    {"INVITE_CREATE", std::bind(&NyaBot::inviteCreate, this, std::placeholders::_1)},
    {"INVITE_DELETE", std::bind(&NyaBot::inviteDelete, this, std::placeholders::_1)},
    {"THREAD_CREATE", std::bind(&NyaBot::threadCreate, this, std::placeholders::_1)},
    {"THREAD_UPDATE", std::bind(&NyaBot::threadUpdate, this, std::placeholders::_1)},
    {"THREAD_DELETE", std::bind(&NyaBot::threadDelete, this, std::placeholders::_1)},
    {"THREAD_LIST_SYNC", std::bind(&NyaBot::threadListSync, this, std::placeholders::_1)},
    {"THREAD_MEMBER_UPDATE", std::bind(&NyaBot::threadMemberUpdate, this, std::placeholders::_1)},
    {"THREAD_MEMBERS_UPDATE", std::bind(&NyaBot::threadMembersUpdate, this, std::placeholders::_1)},
    {"TYPING_START", std::bind(&NyaBot::typingStart, this, std::placeholders::_1)},
    {"RATE_LIMITED", std::bind(&NyaBot::rateLimited, this, std::placeholders::_1)},
    {"USER_UPDATE", std::bind(&NyaBot::userUpdate, this, std::placeholders::_1)},
    {"VOICE_SERVER_UPDATE", std::bind(&NyaBot::voiceServerUpdate, this, std::placeholders::_1)},
    {"VOICE_STATE_UPDATE", std::bind(&NyaBot::voiceStateUpdate, this, std::placeholders::_1)},
    {"VOICE_CHANNEL_START_TIME_UPDATE", std::bind(&NyaBot::voiceChannelStartTimeUpdate, this, std::placeholders::_1)},
    {"VOICE_CHANNEL_STATUS_UPDATE", std::bind(&NyaBot::voiceChannelStatusUpdate, this, std::placeholders::_1)},
  };

  std::unordered_map<std::string, std::unique_ptr<Command>> commands;
  std::mutex guildMemberChunkmtx;
  std::unordered_map<std::string, GuildMemberRequestTask> guildMembersChunkTable;
  Funs funs;
  ImportantApiStuff api;
  std::mutex runMtx;
  std::condition_variable runCv;
  std::vector<Shard> shards;
  InteractionCallbacks iCallbacks;
  int retryAmount = 5;
  std::optional<Presence> presence = std::nullopt;
  std::vector<SlashCommand> slashs;
  std::vector<ContextMenuCommand> ctxMenuCommands;
  std::mutex voiceTaskmtx;
  std::unordered_map<std::string, VoiceCallbacks> voiceTaskList;
  friend RestClient;
  friend EmojiApiRoutes;
  friend Shard;
  friend class VoiceConnection;
};
#endif
