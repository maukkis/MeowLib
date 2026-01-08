#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <cstdint>
#include <cstring>
#include <expected>
#include <optional>

namespace {

enum IpDiscoveryCodes : uint16_t {
  SEND = 0x1,
  RECV = 0x2,
};

constexpr uint8_t bufSize = 74;
constexpr uint8_t portStart = 72;
constexpr uint8_t ipStart = 8;
constexpr uint8_t ipLen = 64;
constexpr uint8_t typeStart = 0;
constexpr uint8_t lenStart = 2;
constexpr uint8_t ssrcStart = 4;



std::optional<IpDiscovery> parseIpDiscovery(char *buf){
  uint16_t type;
  std::memcpy(&type, &buf[typeStart], sizeof(type));
  type = ntohs(type);
  if(type != IpDiscoveryCodes::RECV){
    //make this optionally return an error
    Log::error("something went horribly wrong when doing ip discovery");
    return std::nullopt;
  }
  IpDiscovery a;
  for(size_t i = 0; i < ipLen; ++i){
    a.ip += buf[i+ipStart];
  }
  uint16_t b = 0;
  std::memcpy(&b, &buf[portStart], sizeof(b));
  a.port = ntohs(b);
  return a;
}
}


std::expected<IpDiscovery, std::nullopt_t> VoiceConnection::performIpDiscovery(const VoiceReady& a){
  uSockfd = socket(AF_INET, SOCK_DGRAM, 0);
  api.dest.sin_family = AF_INET;
  api.dest.sin_port = htons(a.port);
  inet_pton(AF_INET, a.ip.c_str(), &api.dest.sin_addr);

  if(::connect(uSockfd, std::bit_cast<sockaddr*>(&api.dest), sizeof(api.dest))){
    Log::error("failed to connect to voice gateway");
    return std::unexpected(std::nullopt);
  }
  Log::dbg("connected to voice udp socket");
  std::array<uint8_t, bufSize> buf{0}; 
  
  uint16_t type = htons(IpDiscoveryCodes::SEND);
  std::memcpy(&buf.at(typeStart), &type, sizeof(type));
  
  uint16_t len = htons(bufSize - ssrcStart);
  std::memcpy(&buf.at(lenStart), &len, sizeof(len));
  
  uint32_t ssrc = htonl(a.ssrc);
  std::memcpy(&buf.at(ssrcStart), &ssrc, sizeof(ssrc));
  #ifdef WIN32
  size_t slen = send(uSockfd, std::bit_cast<char*>(buf.data()), buf.size(), 0);
  #else
  size_t slen = send(uSockfd, buf.data(), buf.size(), 0);
  #endif

  if(slen <= 0){
    Log::error("nothing got sent for ip discovery *bites*");
    return std::unexpected(std::nullopt);
  }

  Log::dbg("sent " + std::to_string(slen) + " bytes");
  char buff[bufSize];
  size_t rlen = recv(uSockfd, buff, sizeof(buff), 0);
  if(rlen <= 0){
    Log::error("didnt receive anything grr");
    return std::unexpected(std::nullopt);
  }


  Log::dbg("received: " + std::to_string(rlen) + " bytes");
  auto ip = parseIpDiscovery(buff);
  if(!ip) return std::unexpected(std::nullopt);
  return *ip;
}
