#include "../../include/nyaBot.h"


void NyaBot::messageCreate(nlohmann::json j){
  if(funs.onMessageCreateF){
    Message msg(j["d"]);
    funs.onMessageCreateF(msg);
  }
}


void NyaBot::messageUpdate(nlohmann::json j){
  if(funs.onMessageUpdateF){
    Message msg(j["d"]);
    funs.onMessageUpdateF(msg);
  }
}

void NyaBot::messageDelete(nlohmann::json j){
  if(funs.onMessageDeleteF){
    j = j["d"];
    MessageDelete a
      {
        .id = j["id"],
        .channelId = j["channel_id"],
        .guildId = j.contains("guild_id") ? std::make_optional(j["guild_id"]) : std::nullopt
      };
    funs.onMessageDeleteF(a);
  }
}


void NyaBot::messageReactionAdd(nlohmann::json j){
  if(funs.onMessageReactionAdd){
    MessageReaction a(j["d"]);
    funs.onMessageReactionAdd(a);
  }
}
