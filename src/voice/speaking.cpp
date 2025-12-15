#include "../../include/voice/voiceconnection.h"
#include <meowHttp/websocket.h>

void VoiceConnection::sendSpeaking(){
  nlohmann::json j;
  j["op"] = VoiceOpcodes::SPEAKING;
  j["d"]["speaking"] = 1;
  j["d"]["delay"] = 0;
  j["d"]["ssrc"] = api.ssrc;
  handle.wsSend(j.dump(), meowWs::meowWS_TEXT);
}


