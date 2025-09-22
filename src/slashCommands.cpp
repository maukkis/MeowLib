#include "../include/slashCommands.h"
#include "../include/nyaBot.h"
#include <nlohmann/json_fwd.hpp>
#include <optional>


SlashCommand::SlashCommand(const std::string_view name, const std::string_view desc, enum IntegrationTypes type) 
  : name{name},
    desc{desc},
    types{static_cast<int>(type)} {}

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
    type{static_cast<int>(type)},
    required{required} {} 


void NyaBot::addSlash(const SlashCommand& slash){
  slashs.emplace_back(slash);
}

std::expected<std::nullopt_t, Error> NyaBot::syncSlashCommands(){
  nlohmann::json json;
  json = nlohmann::json::array();
  for(const auto& it : slashs){
    nlohmann::json a;
    a["name"] = it.name;
    a["type"] = 1;
    a["description"] = it.desc;
    if(it.types == IntegrationTypes::BOTH){
      a["integration_types"] = {0,1};
      a["contexts"] = {0,1,2};
    } else {
      a["integration_types"] = {it.types};
      if(it.types == IntegrationTypes::USER_INSTALL){
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
  auto meow = rest.put(std::format(APIURL "/applications/{}/commands",
                                   api.appId),
                       json.dump());
  if(!meow.has_value() || meow->second != 200){
    Log::error("something went wrong while syncing slash commands " + 
             meow.value_or(std::make_pair("", 0)).first);
    return std::unexpected(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
  }
  return std::nullopt;
}
