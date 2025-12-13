#include "../include/contextMenuCommand.h"
#include <nlohmann/json.hpp>
#include "../include/nyaBot.h"


ContextMenuCommand::ContextMenuCommand(const std::string_view name, ContextMenuTypes type, IntegrationTypes types)
  : name{name}, type{type}, types{types} {}

nlohmann::json ContextMenuCommand::generate() const {
  nlohmann::json j;
  j["name"] = name;
  j["type"] = type;
  if(types == IntegrationTypes::BOTH){
    j["integration_types"] = {0,1};
    j["contexts"] = {0,1,2};
  } else {
    j["integration_types"] = {types};
    if(types == IntegrationTypes::USER_INSTALL){
      j["contexts"] = {2};
    }
    else{
      j["contexts"] = {0};
    }
  }
  return j;
}


void NyaBot::addContextMenuCommand(const ContextMenuCommand& cmd){
  ctxMenuCommands.push_back(cmd);
}
