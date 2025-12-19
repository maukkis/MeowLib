#ifndef _INCLUDE_VOICE_VOICECONNECTION_H
#define _INCLUDE_VOICE_VOICECONNECTION_H


#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <meowHttp/websocket.h>
#include <mutex>
#include <netinet/in.h>
#include <string_view>
#include "../async.h"
#include <expected>
#include <coroutine>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <vector>
#include <deque>
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

struct VoiceData {
  std::vector<uint8_t> payload;
  uint64_t payloadLen{};
  uint64_t duration{};
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
  uint32_t pNonce = 0;
  int seq = -1;
  uint32_t ssrc{};
  uint16_t rtpSeq = 0;
  std::atomic<bool> speaking = false;
  uint32_t timestamp = 0;
  uint64_t heartbeatInterval{};
  std::atomic<bool> stop = false;
  std::array<uint8_t, secretKeyLen> secretKey;
  sockaddr_in dest;
};

class VoiceConnection {
public:
  VoiceConnection(NyaBot *a);
  ~VoiceConnection();
  MeowAsync<void> connect(const std::string_view guildId, const std::string_view channelId);
  void sendOpusData(const uint8_t *opusData, uint64_t duration, uint64_t frameSize);
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
  void udpLoop();
  void sendSilence();
  void addToQueue(const VoiceData& a);
 std::pair<std::vector<uint8_t>, uint32_t> frameRtp(std::vector<uint8_t> a, int dur);
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
  std::thread uth;
  VoiceApiInfo api;
  int uSockfd{};
  std::mutex qmtx;
  std::condition_variable qcv;
  constexpr static std::array<uint8_t, 3> silence {0xf8, 0xff, 0xfe};
  std::deque<VoiceData> voiceDataQueue;
};

#endif
