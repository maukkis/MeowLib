#include "../../include/nyaBot.h"
#include <string>


void NyaBot::ready(nlohmann::json j){
  j = j["d"];
  api.sesId = j["session_id"];
  api.resumeUrl = j["resume_gateway_url"];
  if(api.resumeUrl.back() != '/')
    api.resumeUrl.append("/?v=10&encoding=json");
  else api.resumeUrl.append("?v=10&encoding=json");
  j = j["user"];
  api.appId = j["id"];
  if(funs.onReadyF)
    funs.onReadyF();
}

void NyaBot::resumed(nlohmann::json j){
  Log::info("connection resumed with sequence " + std::to_string(j["s"].get<long>()));
}


