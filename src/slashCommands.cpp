#include "includes/slashCommands.h"
#include "includes/nyaBot.h"
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include "../meowHttp/src/includes/https.h"

SlashCommand::SlashCommand(const std::string& name, const std::string& desc) 
  : name{name},
    desc{desc} {}

SlashCommandParameter& SlashCommand::addParam(const std::string& name, const std::string& value, Types type, bool required){
  params.emplace_back(name, value, type, required);
  return params.back();
}

SlashCommandParameter& SlashCommandParameter::addChoice(const std::string& choice){
  choices.emplace_back(choice);
  return *this;
}

SlashCommandParameter::SlashCommandParameter(const std::string& name, const std::string& desc, Types type, bool required) 
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
    .setUrl("https://discord.com/api/applications/" + appId + "/commands")
    .setHeader("Content-Type: application/json")
    .setHeader("Authorization: Bot " + token)
    .setCustomMethod("PUT")
    .setPostfields(json.dump())
    .setWriteData(&write);
  if(meow.perform() != OK) std::cout << "wooof!" << '\n';
}
