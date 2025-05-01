#include "includes/nyaBot.h"
#include <nlohmann/json.hpp>
#include <string>
#include "includes/slashCommandInt.h"

void NyaBot::onReady(std::function<void ()> f) {
  funs.onReadyF = f;
}

void NyaBot::onSlash(std::function<void (SlashCommandInt)> f) {
  funs.onSlashF = f;
}

void NyaBot::onAutocomplete(std::function<void ()> f) {
  funs.onAutocompleteF = f;
}



static SlashCommandInt constructSlash(nlohmann::json& json){
  const std::string id = json["id"];
  const std::string interactionToken = json["token"];
  const std::string& userId = json["user"]["id"];
  json = json["data"];
  const std::string commandName = json["name"];
  SlashCommandInt slash(id, interactionToken, commandName, std::stoull(userId));
  if(json.find("options") != json.end()){
    for (const auto& it : json["options"]){
      slash.parameters.insert({it["name"], it["value"].get<std::string>()});
    }
  }
  return slash;
}

void NyaBot::interaction(nlohmann::json j){
  j = j["d"];
  int type = j["type"];
  switch(type){
    case APPLICATION_COMMAND:
      auto slash = constructSlash(j);
      if(commands.contains(slash.commandName)){
        commands[slash.commandName]->onCommand(slash);
      }
      else { // command doesnt have a command handler sending it to the default handler
        funs.onSlashF(slash);
      }
    break;
  } 
}


