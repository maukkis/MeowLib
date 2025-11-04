#include "../../include/nyaBot.h"
#include "../../include/typingstart.h"

TypingStart::TypingStart(const nlohmann::json& j){
  channelId = j["channel_id"];
  if(j.contains("guild_id"))
    guildId = j["guild_id"];
  userId = j["user_id"];
  timestamp = j["timestamp"];
  if(j.contains("member")){
    member = User(j["member"]["user"]);
    member->guild = deserializeGuildUser(j["member"]);
  }
}



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

void NyaBot::typingStart(nlohmann::json j){
  if(funs.onTypingStartF){
    TypingStart a(j["d"]);
    funs.onTypingStartF(a);
  }
}
