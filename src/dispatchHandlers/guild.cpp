#include "../../include/nyaBot.h"
#include "../../include/guild.h"


void NyaBot::guildCreate(nlohmann::json j){
  if(funs.onGuildCreateF){
    Guild a = deserializeGuild(j["d"]);
    funs.onGuildCreateF(a);
  }
}

void NyaBot::guildUpdate(nlohmann::json j){
  if(funs.onGuildUpdateF){
    Guild a = deserializeGuild(j["d"]);
    funs.onGuildUpdateF(a);
  }
}

void NyaBot::guildDelete(nlohmann::json j){
  if(funs.onGuildDeleteF){
    UnavailableGuild a = deserializeUnavailableGuild(j["d"]);
    funs.onGuildDeleteF(a);
  }
}

void NyaBot::guildBanAdd(nlohmann::json j){
  if(funs.onGuildBanAddF){
    GuildBan a = deserializeGuildBan(j["d"]);
    funs.onGuildBanAddF(a);
  }
}


void NyaBot::guildBanRemove(nlohmann::json j){
  if(funs.onGuildBanRemoveF){
    GuildBan a = deserializeGuildBan(j["d"]);
    funs.onGuildBanRemoveF(a);
  }
}
