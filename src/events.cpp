#include "includes/nyaBot.h"
#include <nlohmann/json.hpp>
#include "includes/slashCommandInt.h"

void NyaBot::onReady(std::function<void ()> f) {
  onReadyF = f;
}

void NyaBot::onSlash(std::function<void (SlashCommandInt)> f) {
  onSlashF = f;
}

void NyaBot::onAutocomplete(std::function<void ()> f) {
  onAutocompleteF = f;
}

static SlashCommandInt constructSlash(nlohmann::json& json){
  const std::string id = json["id"];
  const std::string interactionToken = json["token"];
  json = json["data"];
  const std::string commandName = json["name"];
  SlashCommandInt slash(id, interactionToken, commandName);
  if(json.find("options") != json.end()){
    for (const auto& it : json["options"]){
      slash.parameters.insert({it["name"], it["value"].get<std::string>()});
    }
  }
  return std::move(slash);
}

void NyaBot::interaction(nlohmann::json j){
  j = j["d"];
  int type = j["type"];
  switch(type){
    case APPLICATION_COMMAND:
      onSlashF(constructSlash(j));
  } 
}


