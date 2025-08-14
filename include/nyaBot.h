#ifndef nyaBot_H
#define nyaBot_H
#include "../meowHttp/src/includes/websocket.h"
#include "buttonInteraction.h"
#include "queue.h"
#include "restclient.h"
#include "selectInteraction.h"
#include "slashCommandInt.h"
#include "slashCommands.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <atomic>
#include "log.h"
#include <nlohmann/json.hpp>
#include <thread>
#include <iostream>
#include <type_traits>
#include <unordered_map>
#include "commandHandling.h"



struct ImportantApiStuff {
  std::string token;
  uint64_t interval;
  size_t sequence{0};
  std::string appId;
  std::string sesId;
  std::string resumeUrl;
};

struct Funs {
  std::function<void(SlashCommandInt&)> onSlashF = {};
  std::function<void()> onReadyF = {};
  std::function<void()> onAutocompleteF = {};
  std::function<void(ButtonInteraction&)> onButtonF = {};
  std::function<void(SelectInteraction&)> onSelectF = {};
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
};


template<typename T>
concept CommandHandler = std::is_base_of_v<Command, T>;


class NyaBot {
public:
  NyaBot();
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

  void enableDebugLogging(){
    Log::enabled = true;
  }

  template<CommandHandler Command, typename... Args>
  void addCommandHandler(const std::string& commandName, Args&&... args){
    commands[commandName] = std::make_unique<Command>(std::forward<Args>(args)...);
  }

  void onReady(std::function<void()> f);
  void onSlash(std::function<void(SlashCommandInt&)> f);
  void onAutocomplete(std::function<void()> f);
  void onButtonPress(std::function<void(ButtonInteraction&)> f);
  void onSelectInteraction(std::function<void(SelectInteraction&)> f);
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
  void syncSlashCommands();
  RestClient rest {this};
private:
  void listen();
  void connect();
  void sendIdent();
  void getHeartbeatInterval();
  void routeInteraction(ButtonInteraction& interaction);
  void routeInteraction(SelectInteraction& interaction);
  void ready(nlohmann::json j);
  void interaction(nlohmann::json j);
  void resumed(nlohmann::json j);
  void messageCreate(nlohmann::json j);
  void messageUpdate(nlohmann::json j);
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
    {"MESSAGE_UPDATE", std::bind(&NyaBot::messageUpdate, this, std::placeholders::_1)}
  };
  
  std::unordered_map<std::string, std::unique_ptr<Command>> commands;
  Funs funs;
  ImportantApiStuff api;
  InteractionCallbacks iCallbacks;
  ThreadSafeQueue<std::string> queue;

  std::vector<SlashCommand> slashs;
  friend RestClient;
};
#endif
