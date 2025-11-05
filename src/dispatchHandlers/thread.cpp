#include "../../include/nyaBot.h"
#include <nlohmann/json.hpp>

void NyaBot::threadCreate(nlohmann::json j){
  Channel a(j["d"]);
  channel.cache.insert(a.id, a);
  if(funs.onThreadCreateF){
    funs.onThreadCreateF(a);
  }
}


void NyaBot::threadUpdate(nlohmann::json j){
  Channel a(j["d"]);
  channel.cache.insert(a.id, a);
  if(funs.onThreadUpdateF){
    funs.onThreadUpdateF(a);
  }
}


void NyaBot::threadDelete(nlohmann::json j){
  Channel a(j["d"]);
  channel.cache.insert(a.id, a);
  if(funs.onThreadDeleteF){
    funs.onThreadDeleteF(a);
  }
}

void NyaBot::threadListSync(nlohmann::json j){
  if(funs.onThreadListSyncF){
    ThreadListSync a(j["d"]);
    funs.onThreadListSyncF(a);
  }
}

void NyaBot::threadMemberUpdate(nlohmann::json j){
  if(funs.onThreadMemberUpdateF){
    ThreadMember a(j["d"]);
    funs.onThreadMemberUpdateF(a);
  }
}

void NyaBot::threadMembersUpdate(nlohmann::json j){
  if(funs.onThreadMembersUpdateF){
    ThreadMembersUpdate a(j["d"]);
    funs.onThreadMembersUpdateF(a);
  }
}
