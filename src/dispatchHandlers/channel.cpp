#include "../../include/nyaBot.h"

void NyaBot::channelCreate(nlohmann::json j){
  if(funs.onChannelCreateF){
    Channel a = deserializeChannel(j["d"]);
    funs.onChannelCreateF(a);
  }
}

void NyaBot::channelDelete(nlohmann::json j){
  if(funs.onChannelDeleteF){
    Channel a = deserializeChannel(j["d"]);
    funs.onChannelDeleteF(a);
  }
}

void NyaBot::channelUpdate(nlohmann::json j){
  if(funs.onChannelUpdateF){
    Channel a = deserializeChannel(j["d"]);
    funs.onChannelUpdateF(a);
  }
}
