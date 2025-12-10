#ifndef _INCLUDE_SHARD_H
#define _INCLUDE_SHARD_H
#include <cstdint>
#include <meowHttp/websocket.h>
#include <thread>
#include <nlohmann/json_fwd.hpp>
#include <expected>
#include "queue.h"

enum class GatewayStates {
  UNAUTHENTICATED,
  AUTHENTICATION_SENT,
  RESUME_SENT,
  READY,
};

class NyaBot;


struct State {
  State() = default;
  State(State&& a){
    state = a.state.load();
    sequence = a.sequence;
    interval = a.interval;
    ping = a.ping.load();
    resumeUrl = a.resumeUrl;
    sesId = a.sesId;
    shardId = a.shardId;
    noShard = a.noShard;
  }
  size_t sequence{0};
  uint64_t interval{};
  std::atomic<int> ping = -1;
  std::string resumeUrl;
  std::string sesId;
  std::atomic<GatewayStates> state = GatewayStates::UNAUTHENTICATED;
  int shardId{};
  bool noShard = false;
};

class Shard {
public:
  Shard(NyaBot *bot, int shardId, bool noShard = false);
  ~Shard();
  void close();
  Shard(const Shard&) = delete;
  Shard& operator=(const Shard&) = delete;
  Shard(Shard&&) = default;
  GatewayStates getState();
private:
  void listen();
  meow reconnect(bool resume = false);
  void routeEvent(const nlohmann::json& j);
  void sendIdent();
  void getHeartbeatInterval();
  std::expected<std::nullopt_t, meow> connect();
  std::thread th;
  meowWs::Websocket handle;
  NyaBot *bot = nullptr;
  State api;
  ThreadSafeQueue<std::string> queue;
  friend NyaBot;
};

#endif
