#include "../include/slashCommands.h"
#include "../include/nyaBot.h"
#include <nlohmann/json_fwd.hpp>
#include "../meowHttp/src/includes/https.h"

SlashCommand::SlashCommand(const std::string_view name, const std::string_view desc, enum IntegrationTypes type) 
  : name{name},
    desc{desc},
    types{type} {}

SlashCommand& SlashCommand::addParam(SlashCommandParameter a){
  params.emplace_back(a);
  return *this;
}

SlashCommandParameter& SlashCommandParameter::addChoice(const std::string_view choice){
  choices.emplace_back(choice);
  return *this;
}

SlashCommandParameter::SlashCommandParameter(const std::string_view name, const std::string_view desc, Types type, bool required) 
  : name{name},
    desc{desc},
    type{type},
    required{required} {} 


void NyaBot::addSlash(const SlashCommand& slash){
  slashs.emplace_back(slash);
}

void NyaBot::syncSlashCommands(){
  nlohmann::json json;
  json = nlohmann::json::array();
  for(const auto& it : slashs){
    nlohmann::json a;
    a["name"] = it.name;
    a["type"] = 1;
    a["description"] = it.desc;
    if(it.types == BOTH){
      a["integration_types"] = {0,1};
      a["contexts"] = {0,1,2};
    } else {
      a["integration_types"] = {it.types};
      if(it.types == USER_INSTALL){
        a["contexts"] = {2};
      }
      else{
        a["contexts"] = {0};
      }
    }
    a["options"] = nlohmann::json::array();
    for(const auto& opt : it.params){
      nlohmann::json p;
      p["name"] = opt.name;
      p["description"] = opt.desc;
      p["type"] = opt.type;
      p["required"] = opt.required;
      p["choices"] = nlohmann::json::array();
      for(const auto& choice : opt.choices){
        nlohmann::json c;
        c["name"] = choice;
        c["value"] = choice;
        p["choices"].emplace_back(c);
      }
      a["options"].emplace_back(p);
    }
    json.emplace_back(a);
  }
  std::string write;
  auto meow = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/applications/" + api.appId + "/commands")
    .setHeader("content-type: application/json")
    .setHeader("authorization: Bot " + api.token)
    .setCustomMethod("PUT")
    .setPostfields(json.dump())
    .setWriteData(&write);
  if(meow.perform() != OK || meow.getLastStatusCode() != HTTPCODES::HTTP_OK)
    Log::Log("failed to update slashcommands\n" + write);
}
