#include "../../include/voice/voiceconnection.h"
#include <meowHttp/websocket.h>
#include <nlohmann/json.hpp>
#include <unordered_set>
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
  return "";
}
}



void VoiceConnection::handleSessionDescription(const nlohmann::json& j){
  if(j["d"]["mode"] != cipherToStr(api.cipher)){
    Log::error("invalid cipher from voice gw");
    api.stop = true;
    return;
  }
  Log::dbg("session description valid");
  api.secretKey = j["d"]["secret_key"];
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
  api.ssrc = r.ssrc;
  std::unordered_set<std::string> modes(r.modes.begin(), r.modes.end());
  if(modes.contains(cipherToStr(Ciphers::aead_aes256_gcm_rtpsize)))
    api.cipher = Ciphers::aead_aes256_gcm_rtpsize;
  else
    api.cipher = Ciphers::aead_xchacha20_poly1305_rtpsize;

  Log::dbg(std::format("using cipher: {}", cipherToStr(api.cipher)));
  auto a = performIpDiscovery(r);
  if(!a){
    Log::error("failed to perform ip discovery");
    api.stop = true;
    return;
  }
  sendSelectProtocol(*a);
  sendSpeaking();
  api.state = VoiceGatewayState::READY;
  udpInterrupt = false;
  qcv.notify_all();
  if(!uth.joinable())
    uth = std::thread(&VoiceConnection::udpLoop, this); 
}
