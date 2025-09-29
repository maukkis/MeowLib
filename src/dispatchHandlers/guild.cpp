#include "../../include/nyaBot.h"
#include "../../include/guild.h"


void NyaBot::guildCreate(nlohmann::json j){
  if(funs.onGuildCreateF){
    Guild a = deserializeGuild(j["d"]);
    funs.onGuildCreateF(a);;
  }
}
