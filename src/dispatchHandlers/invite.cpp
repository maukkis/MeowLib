#include "../../include/nyaBot.h"
#include "../../include/invite.h"


void NyaBot::inviteCreate(nlohmann::json j){
  if(funs.onInviteCreateF){
    InviteCreateEvent a(j["d"]);
    funs.onInviteCreateF(a);
  }
}


void NyaBot::inviteDelete(nlohmann::json j){
  if(funs.onInviteDeleteF){
    InviteDeleteEvent a(j["d"]);
    funs.onInviteDeleteF(a);
  }
}
