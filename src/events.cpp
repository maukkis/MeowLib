#include "includes/nyaBot.h"
#include <nlohmann/json.hpp>
#include "includes/slashcommand.h"

void NyaBot::onReady(std::function<void ()> f) {
  onReadyF = f;
}

void NyaBot::onSlash(std::function<void (SlashCommand)> f) {
  onSlashF = f;
}

void NyaBot::onAutocomplete(std::function<void ()> f) {
  onAutocompleteF = f;
}

static SlashCommand constructSlash(nlohmann::json& json){
  const std::string id = json["id"];
  const std::string interactionToken = json["token"];
  json = json["data"];
  const std::string commandName = json["name"];
  SlashCommand slash(id, interactionToken, commandName);
  if(json.find("options") != json.end()){
    json = json["options"];
    for (const auto& it : json){
      slash.parameters.push_back(std::make_pair(it["name"], it["value"]));
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


