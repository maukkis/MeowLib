#ifndef nyaBot_H
#define nyaBot_H
#include "../meowHttp/src/includes/websocket.h"
#include "buttonInteraction.h"
#include "channel.h"
#include "emoji.h"
#include "guild.h"
#include "message.h"
#include "modalInteraction.h"
#include "queue.h"
#include "restclient.h"
#include "automod.h"
#include "selectInteraction.h"
#include "slashCommandInt.h"
#include "presence.h"
#include "slashCommands.h"
#include <cstdint>
#include <expected>
#include <functional>
#include <future>
#include <nlohmann/json_fwd.hpp>
#include "error.h"
#include <string>
#include <atomic>
#include "log.h"
#include <nlohmann/json.hpp>
#include <type_traits>
#include <unordered_map>
#include "commandHandling.h"
#include "typingstart.h"
#include "user.h"



struct ImportantApiStuff {
  std::string token;
  uint64_t interval;
  size_t sequence{0};
  std::string appId;
  std::string sesId;
  std::string resumeUrl;
  int intents = 0;
};

struct Funs {
  std::function<void(SlashCommandInt&)> onSlashF = {};
  std::function<void()> onReadyF = {};
  std::function<void()> onAutocompleteF = {};
  std::function<void(ButtonInteraction&)> onButtonF = {};
  std::function<void(SelectInteraction&)> onSelectF = {};
  std::function<void(ModalInteraction&)> onModalF = {};


  std::function<void(Message&)> onMessageCreateF = {};
  std::function<void(Message&)> onMessageUpdateF = {};
  std::function<void(MessageDelete&)> onMessageDeleteF = {};
  std::function<void(MessageReaction&)> onMessageReactionAddF = {};

  std::function<void(Guild&)> onGuildCreateF = {};
  std::function<void(Guild&)> onGuildUpdateF = {};
  std::function<void(UnavailableGuild&)> onGuildDeleteF = {};
  std::function<void(GuildBan&)> onGuildBanAddF = {};
  std::function<void(GuildBan&)> onGuildBanRemoveF = {};
  std::function<void(User&)> onGuildMemberRemoveF = {};
  std::function<void(User&)> onGuildMemberAddF = {};
  std::function<void(User&)> onGuildMemberUpdateF = {};

  std::function<void(Channel&)> onChannelCreateF = {};
  std::function<void(Channel&)> onChannelUpdateF = {};
  std::function<void(Channel&)> onChannelDeleteF = {};

  std::function<void(TypingStart&)> onTypingStartF = {};
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
  /// @brief Registers a slash command with the bot.
  /// @param slash SlashCommand object
  ///
  void addSlash(const SlashCommand& slash);
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


  void enableDebugLogging(){
    Log::enabled = true;
  }
  
  ///@brief sets the amount of times to retry a failed request due to an I/O error
  ///@param amount amount to set
  void setRetryAmount(int amount){
    retryAmount = amount;
  }


  ///
  /// @brief changes the presence of the bot user the gateway connection must be on ready state for this to be called
  /// @param p presence object
  ///
  void changePresence(const Presence& p);
  void onReady(std::function<void()> f);
  void onSlash(std::function<void(SlashCommandInt&)> f);
  void onAutocomplete(std::function<void()> f);
  void onButtonPress(std::function<void(ButtonInteraction&)> f);
  void onSelectInteraction(std::function<void(SelectInteraction&)> f);
  void onModalSubmit(std::function<void(ModalInteraction&)> f);

  void onMessageCreate(std::function<void(Message&)> f);
  void onMessageUpdate(std::function<void(Message&)> f);
  void onMessageDelete(std::function<void(MessageDelete&)> f);
  void onMessageReactionAdd(std::function<void(MessageReaction&)> f);

  void onGuildCreate(std::function<void(Guild&)> f);
  void onGuildUpdate(std::function<void(Guild&)> f);
  void onGuildDelete(std::function<void(UnavailableGuild&)> f);
  void onGuildBanAdd(std::function<void(GuildBan&)> f);
  void onGuildBanRemove(std::function<void(GuildBan&)> f);
  void onGuildMemberRemove(std::function<void(User&)> f);
  void onGuildMemberAdd(std::function<void(User&)> f);
  void onGuildMemberUpdate(std::function<void(User&)> f);

  void onChannelCreate(std::function<void(Channel&)> f);
  void onChannelUpdate(std::function<void(Channel&)> f);
  void onChannelDelete(std::function<void(Channel&)> f);


  void onTypingStart(std::function<void(TypingStart&)> f);
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
  std::expected<std::nullopt_t, Error> syncSlashCommands();

  /// @brief request guild members from a guild from the gw
  /// @param guildId guild to request from
  /// @param query what the username has to start with leave as "" for all
  /// @param limit max amount of members to receive leave 0 for no limit
  /// @returns future to a std::vector<User> with the guild field filled up
  std::future<std::vector<User>> requestGuildMembers(const std::string_view guildId,
                                                     const std::string_view query,
                                                     const int limit);
  RestClient rest {this};
  UserApiRoutes user{this};
  GuildApiRoutes guild{this};
  EmojiApiRoutes emoji{this};
  MessageApiRoutes message{this};
  AutoModApiRoutes automod{this};
private:
  void listen();
  void connect();
  void sendIdent();
  void getHeartbeatInterval();
  void routeInteraction(ButtonInteraction& interaction);
  void routeInteraction(SelectInteraction& interaction);
  void routeInteraction(ModalInteraction& interaction);

  void ready(nlohmann::json j);
  void interaction(nlohmann::json j);
  void resumed(nlohmann::json j);

  void messageCreate(nlohmann::json j);
  void messageUpdate(nlohmann::json j);
  void messageDelete(nlohmann::json j);
  void messageReactionAdd(nlohmann::json j);

  void guildCreate(nlohmann::json j);
  void guildUpdate(nlohmann::json j);
  void guildDelete(nlohmann::json j);
  void guildBanAdd(nlohmann::json j);
  void guildBanRemove(nlohmann::json j);
  void guildMemberRemove(nlohmann::json j);
  void guildMemberAdd(nlohmann::json j);
  void guildMemberUpdate(nlohmann::json j);
  void guildMemberChunk(nlohmann::json j);

  void channelCreate(nlohmann::json j);
  void channelUpdate(nlohmann::json j);
  void channelDelete(nlohmann::json j);

  void typingStart(nlohmann::json j);

  meow reconnect(bool resume);
  static void signalHandler(int){
    a->stop = true;
  }
  inline static NyaBot *a = nullptr;
  std::atomic<bool> stop;
  meowWs::Websocket handle;
 
  std::unordered_map<std::string_view, std::function<void(nlohmann::json)>> dispatchHandlers {
    {"INTERACTION_CREATE", std::bind(&NyaBot::interaction, this, std::placeholders::_1)},
    {"READY", std::bind(&NyaBot::ready, this, std::placeholders::_1)},
    {"RESUMED", std::bind(&NyaBot::resumed, this, std::placeholders::_1)},
    {"MESSAGE_CREATE", std::bind(&NyaBot::messageCreate, this, std::placeholders::_1)},
    {"MESSAGE_UPDATE", std::bind(&NyaBot::messageUpdate, this, std::placeholders::_1)},
    {"MESSAGE_DELETE", std::bind(&NyaBot::messageDelete, this, std::placeholders::_1)},
    {"GUILD_CREATE", std::bind(&NyaBot::guildCreate, this, std::placeholders::_1)},
    {"GUILD_UPDATE", std::bind(&NyaBot::guildUpdate, this, std::placeholders::_1)},
    {"GUILD_DELETE", std::bind(&NyaBot::guildDelete, this, std::placeholders::_1)},
    {"GUILD_BAN_ADD", std::bind(&NyaBot::guildBanAdd, this, std::placeholders::_1)},
    {"GUILD_BAN_REMOVE", std::bind(&NyaBot::guildBanRemove, this, std::placeholders::_1)},
    {"GUILD_MEMBER_REMOVE", std::bind(&NyaBot::guildMemberRemove, this, std::placeholders::_1)},
    {"GUILD_MEMBER_ADD", std::bind(&NyaBot::guildMemberAdd, this, std::placeholders::_1)},
    {"GUILD_MEMBER_UPDATE", std::bind(&NyaBot::guildMemberUpdate, this, std::placeholders::_1)},
    {"GUILD_MEMBERS_CHUNK", std::bind(&NyaBot::guildMemberChunk, this, std::placeholders::_1)},
    {"MESSAGE_REACTION_ADD", std::bind(&NyaBot::messageReactionAdd, this, std::placeholders::_1)},
    {"CHANNEL_CREATE", std::bind(&NyaBot::channelCreate, this, std::placeholders::_1)},
    {"CHANNEL_UPDATE", std::bind(&NyaBot::channelUpdate, this, std::placeholders::_1)},
    {"CHANNEL_DELETE", std::bind(&NyaBot::channelDelete, this, std::placeholders::_1)},
    {"TYPING_START", std::bind(&NyaBot::typingStart, this, std::placeholders::_1)},
  };

  std::unordered_map<std::string, std::unique_ptr<Command>> commands;

  std::mutex guildMemberChunkmtx;
  std::unordered_map<std::string, GuildMemberRequestData> guildMembersChunkTable;
  Funs funs;
  ImportantApiStuff api;
  InteractionCallbacks iCallbacks;
  ThreadSafeQueue<std::string> queue;
  int retryAmount = 5;
  std::vector<SlashCommand> slashs;
  friend RestClient;
  friend EmojiApiRoutes;
};
#endif
