#include "../../include/nyaBot.h"
#include <nlohmann/json.hpp>

void NyaBot::threadCreate(nlohmann::json j){
  if(funs.onThreadCreateF){
    Channel a = deserializeChannel(j["d"]);
    funs.onThreadCreateF(a);
  }
}


void NyaBot::threadUpdate(nlohmann::json j){
  if(funs.onThreadUpdateF){
    Channel a = deserializeChannel(j["d"]);
    funs.onThreadUpdateF(a);
  }
}


void NyaBot::threadDelete(nlohmann::json j){
  if(funs.onThreadDeleteF){
    Channel a = deserializeChannel(j["d"]);
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
