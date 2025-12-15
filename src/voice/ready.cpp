#include "../../include/voice/voiceconnection.h"
#include <meowHttp/websocket.h>
#include <nlohmann/json.hpp>
#include "../../include/log.h"

VoiceReady::VoiceReady(const nlohmann::json& j){
  ssrc = j["ssrc"];
  ip = j["ip"];
  port = j["port"];
  modes = j["modes"];
}

namespace {
std::string cipherToStr(Ciphers cipher){
  switch(cipher){
    case Ciphers::aead_aes256_gcm_rtpsize:
      return "aead_aes256_gcm_rtpsize";
    break;
    case Ciphers::aead_xchacha20_poly1305_rtpsize:
      return "aead_xchacha20_poly1305_rtpsize";
    break;
  }
}
}


void VoiceConnection::sendSelectProtocol(const IpDiscovery& i){
  nlohmann::json j;
  j["op"] = VoiceOpcodes::SELECT_PROTOCOL;
  j["d"]["protocol"] = "udp";
  nlohmann::json data;
  data["address"] = i.ip;
  data["port"] = i.port;
  data["mode"] = cipherToStr(api.cipher);
  j["d"]["data"] = data;
  handle.wsSend(j.dump(), meowWs::meowWS_TEXT);
}


void VoiceConnection::handleReady(const nlohmann::json& j){
  VoiceReady r(j["d"]);
  bool contains = false;
  for(const auto& c : r.modes){
    if(c == "aead_aes256_gcm_rtpsize"){
      contains = true;
      api.cipher = Ciphers::aead_aes256_gcm_rtpsize;
    }
  }
  if(!contains){
    Log::dbg("couldnt find a supported cipher suite");
    api.stop = true;
    close();
  }
  auto a = performIpDiscovery(r);
  if(!a){
    Log::error("failed to perform ip discovery");
    api.stop = true;
    close();
  }
  sendSelectProtocol(*a);
}
