#ifndef nyaBot_H
#define nyaBot_H
#include "../../meowHttp/src/includes/websocket.h"
#include "slashCommandInt.h"
#include "slashCommands.h"
#include <functional>
#include <mutex>
#include <string>
#include <atomic>
#include <nlohmann/json.hpp>
#include <thread>


template<typename F, typename... Args>
auto runOnce(F&& f, Args&&... a) {
  static bool ran = false;
  if(ran) return;
  ran = true;
  std::invoke(std::forward<F>(f), std::forward<Args>(a)...);
}


class NyaBot {
public:
  NyaBot();
  ~NyaBot();

  void run(const std::string& token);
  void addSlash(const SlashCommand& slash);
  void onReady(std::function<void()> f);
  void onSlash(std::function<void(SlashCommandInt)> f);
  void onAutocomplete(std::function<void()> f);
  void syncSlashCommands();
private:
  void sendHeartbeat();
  void listen();
  void connect();
  void sendIdent();
  void getHeartbeatInterval();
  void interaction(nlohmann::json j);
  meow reconnect(const std::string& sesId, std::string& reconnectUrl, bool resume);
  static void signalHandler(int){
    a->stop.store(true);
  }
  inline static NyaBot *a = nullptr;
  std::function<void(SlashCommandInt)> onSlashF = {};
  std::function<void()> onReadyF = {};
  std::function<void()> onAutocompleteF = {};
  std::atomic<bool> stop{false};
  std::string token;
  meowWs::Websocket handle;
  std::thread hbT;
  
  std::mutex handleLock;
  std::uint64_t interval;
  size_t sequence{0};
  std::string appId;
  std::vector<SlashCommand> slashs;
};
#endif
