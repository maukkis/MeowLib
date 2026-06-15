#include "../include/contextMenuCommand.h"
#include <nlohmann/json.hpp>
#include "../include/nyaBot.h"


ContextMenuCommand::ContextMenuCommand(const std::string_view name, ContextMenuTypes type, IntegrationTypes itypes)
  : name{name}, type{type}
{
  if(itypes == IntegrationTypes::BOTH){
    types = {0,1};
    contexts = {InteractionContexts::GUILD, InteractionContexts::BOT_DM, InteractionContexts::PRIVATE_CHANNEL};
  } else {
    types = {static_cast<int>(itypes)};
    if(itypes == IntegrationTypes::USER_INSTALL){
      contexts = {InteractionContexts::PRIVATE_CHANNEL};
    }
    else{
      contexts = {InteractionContexts::GUILD};
    }
  }
}

ContextMenuCommand& ContextMenuCommand::setContexts(const std::vector<InteractionContexts> contexts){
  this->contexts = contexts;
  return *this;
}

nlohmann::json ContextMenuCommand::generate() const {
  nlohmann::json j;
  j["name"] = name;
  j["type"] = type;
  j["contexts"] = contexts;
  j["integration_types"] = types;
  return j;
}


void NyaBot::addContextMenuCommand(const ContextMenuCommand& cmd){
  ctxMenuCommands.push_back(cmd);
}
