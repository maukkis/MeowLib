#include "../../include/nyaBot.h"

void NyaBot::autoModerationRuleCreate(nlohmann::json j){
  if(funs.onAutoModerationRuleCreateF){
    AutoModRule a(j["d"]);
    funs.onAutoModerationRuleCreateF(a);
  }
}


void NyaBot::autoModerationRuleUpdate(nlohmann::json j){
  if(funs.onAutoModerationRuleUpdateF){
    AutoModRule a(j["d"]);
    funs.onAutoModerationRuleUpdateF(a);
  }
}


void NyaBot::autoModerationRuleDelete(nlohmann::json j){
  if(funs.onAutoModerationRuleDeleteF){
    AutoModRule a(j["d"]);
    funs.onAutoModerationRuleDeleteF(a);
  }
}

void NyaBot::autoModerationActionExecution(nlohmann::json j){
  if(funs.onAutoModerationActionExecutionF){
    AutoModActionExecution a(j["d"]);
    funs.onAutoModerationActionExecutionF(a);
  }
}
