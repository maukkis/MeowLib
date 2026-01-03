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
  CLIENT_CONNECT = 11,
};

constexpr int msToNs = 1000000;
constexpr int rtpFrameSize = 12;
constexpr int aes256GcmTagSize = 16;
constexpr int aes256GcmAADSize = 12;
constexpr int sampleRate = 48000;
constexpr int frameSize = 960;

struct IpDiscovery {
  std::string ip;
  uint16_t port = 0;
};

struct rtpHeader {
  uint8_t magic1;
  uint8_t magic2;
  uint16_t seq;
  uint32_t ts;
  uint32_t ssrc;
  rtpHeader(uint16_t seq, uint32_t ts, uint32_t ssrc) : magic1(0x80), magic2(0x78), seq(htons(seq)), ts(htonl(ts)), ssrc(htonl(ssrc)) {}
  rtpHeader() = default;
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

enum class VoiceGatewayState {
  DISONNECTED,
  CONNECTED,
  READY,
  CHANGING_VOICE_SERVER,
};

struct VoiceApiInfo {
  std::string guildId;
  Ciphers cipher;
  uint32_t pNonce = 0;
  int seq = -1;
  uint32_t ssrc{};
  uint16_t rtpSeq = 0;
  uint32_t timestamp = 0;
  uint64_t heartbeatInterval{};
  std::atomic<bool> stop = false;
  std::array<uint8_t, secretKeyLen> secretKey;
  VoiceGatewayState state = VoiceGatewayState::DISONNECTED;
  sockaddr_in dest;
};

class VoiceConnection {
public:
  VoiceConnection(NyaBot *a);
  ~VoiceConnection();
  void connect(const std::string_view guildId, const std::string_view channelId);
  /// @brief encodes encrypts and sends off pcmData
  /// @param pcmData pointer to pcmData
  /// @param len length of pcm data
  /// @attention slow!!!! as we have to make a copy of the data and opus encode it this function will also block until all the data is encoded and encrypted
  std::expected<std::nullopt_t, int> sendPcmData(const uint8_t* pcmData, size_t len);
  void sendOpusData(const uint8_t *opusData, uint64_t duration, uint64_t frameSize);
  void flush();
  void disconnect();
private:
  VoiceInfo& getConnectInfo(const std::string& guildId, const std::string_view channelId);
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
  void reconnect(bool resume = false, bool waitForNewVoice = false);
  void voiceServerUpdate(VoiceInfo&);
  void addToQueue(const VoiceData& a);
 std::pair<std::vector<uint8_t>, uint32_t> frameRtp(std::vector<uint8_t>& a, int dur);
  void handleSessionDescription(const nlohmann::json& j);
  std::expected<IpDiscovery, std::nullopt_t> performIpDiscovery(const VoiceReady& a);
  int aeadAes256GcmRtpsizeEncrypt(uint8_t *pt,
                                  int ptLen,
                                  uint8_t *key,
                                  uint8_t *iv,
                                  uint8_t *aad,
                                  int aadlen,
                                  uint8_t *ct);
  void closer(bool forget);

  meowWs::Websocket handle;
  NyaBot *bot = nullptr;
  std::thread th;
  std::thread uth;
  VoiceApiInfo api;
  int uSockfd{};
  std::mutex voiceServerUpdatemtx;
  VoiceInfo voiceinfo;
  std::condition_variable voiceServerUpdatecv;
  std::atomic<bool> voiceServerUpdateFlag = false;
  std::atomic<bool> udpInterrupt = false;
  std::mutex qmtx;
  std::condition_variable qcv;
  std::mutex fmtx;
  std::condition_variable fcv;
  inline static std::vector<uint8_t> silence {0xf8, 0xff, 0xfe};
  std::deque<VoiceData> voiceDataQueue;
};

#endif
