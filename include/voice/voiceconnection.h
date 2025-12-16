#ifndef _INCLUDE_VOICE_VOICECONNECTION_H
#define _INCLUDE_VOICE_VOICECONNECTION_H


#include <array>
#include <cstdint>
#include <meowHttp/websocket.h>
#include <string_view>
#include "../async.h"
#include <expected>
#include <coroutine>
#include <nlohmann/json.hpp>
#include "../queue.h"
#include "../voice.h"



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



struct IpDiscovery {
  std::string ip;
  uint16_t port = 0;
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

constexpr uint8_t secretKeyLen = 32;

struct VoiceApiInfo {
  std::string guildId;
  Ciphers cipher;
  uint32_t pNonce = 69;
  int seq = -1;
  int ssrc{};
  uint64_t heartbeatInterval{};
  std::atomic<bool> stop = false;
  std::array<uint8_t, secretKeyLen> secretKey;
};

class VoiceConnection {
public:
  VoiceConnection(NyaBot *a);
  ~VoiceConnection();
  MeowAsync<void> connect(const std::string_view guildId, const std::string_view channelId);
  void sendOpusData(uint8_t *opusData, uint64_t opusLen);
  void disconnect();
private:
  VoiceTask& getConnectInfo(const std::string& guildId, const std::string_view channelId);
  void getHello();
  void close();
  void sendIdentify(const VoiceInfo& info);
  void listen();
  void handleReady(const nlohmann::json& j);
  void sendSelectProtocol(const IpDiscovery& i);
  void sendSpeaking();
  void sendVoiceData();
  void handleSessionDescription(const nlohmann::json& j);
  std::expected<IpDiscovery, std::nullopt_t> performIpDiscovery(const VoiceReady& a);
  int aeadAes256GcmRtpsizeEncrypt(uint8_t *pt,
                                  int ptLen,
                                  uint8_t *key,
                                  uint8_t *iv,
                                  uint8_t *aad,
                                  int aadlen,
                                  uint8_t *ct);

  meowWs::Websocket handle;
  NyaBot *bot = nullptr;
  std::thread th;
  VoiceApiInfo api;
  int uSockfd{};
  ThreadSafeQueue<std::vector<uint8_t>> voiceDataQueue;
};

#endif
