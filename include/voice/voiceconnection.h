#ifndef _INCLUDE_VOICE_VOICECONNECTION_H
#define _INCLUDE_VOICE_VOICECONNECTION_H


#include <cstdint>
#include <meowHttp/websocket.h>
#include <openssl/evp.h>
#include <string_view>
#include "../async.h"
#include <expected>
#include <coroutine>
#include <nlohmann/json.hpp>



class NyaBot;

enum VoiceOpcodes {
  IDENTIFY,
  SELECT_PROTOCOL,
  READY,
  HEARTBEAT,
  SESSION_DESCRIPTION,
  SPEAKING,
  HEARTBEAT_ACK,
};


struct VoiceInfo {
  std::string endpoint;
  std::string guildId;
  std::string token;
  std::string sessionId;
};

struct IpDiscovery {
  std::string ip;
  uint16_t port = 0;
};

struct VoiceTask {
  VoiceInfo info;
  std::mutex mtx;
  std::coroutine_handle<> hp;
  bool await_ready() const noexcept {
    return !info.endpoint.empty() && !info.guildId.empty()
            && !info.token.empty() && !info.sessionId.empty();
  }
  void await_suspend(std::coroutine_handle<> handle) noexcept {
    hp = handle;
  }
  VoiceTask& operator=([[maybe_unused]]const VoiceTask& a){return *this;}
  VoiceInfo await_resume() const noexcept {
    return info;
  }
};

struct VoiceReady {
  VoiceReady(const nlohmann::json& j);
  int ssrc;
  std::string ip;
  int port;
  std::vector<std::string> modes;
};

enum class Ciphers {
  aead_aes256_gcm_rtpsize,
  aead_xchacha20_poly1305_rtpsize,
};


struct VoiceApiInfo {
  Ciphers cipher;
  int seq = -1;
  uint64_t heartbeatInterval{};
  std::atomic<bool> stop = false;
};

class VoiceConnection {
public:
  VoiceConnection(NyaBot *a);
  ~VoiceConnection();
  MeowAsync<void> connect(const std::string_view guildId, const std::string_view channelId);
private:
  VoiceTask& getConnectInfo(const std::string& guildId, const std::string_view channelId);
  void getHello();
  void close();
  void sendIdentify(const VoiceInfo& info);
  void listen();
  void handleReady(const nlohmann::json& j);
  void sendSelectProtocol(const IpDiscovery& i);
  std::expected<IpDiscovery, std::nullopt_t> performIpDiscovery(const VoiceReady& a);
  meowWs::Websocket handle;
  NyaBot *bot = nullptr;
  std::thread th;
  VoiceApiInfo api;
  int uSockfd{};
};

#endif
