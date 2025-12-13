#include "../include/slashCommands.h"
#include "../include/nyaBot.h"
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <format>


SlashCommand::SlashCommand(const std::string_view name, const std::string_view desc, enum IntegrationTypes type) 
  : name{name},
    desc{desc},
    types{static_cast<int>(type)} {}

SlashCommand& SlashCommand::addParam(const SlashCommandParameter& a){
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


void NyaBot::addSlash(SlashCommand& slash){
  if(slash.handler)
    addCommandHandler(slash.name, std::move(slash.handler));
  slashs.push_back(std::move(slash));
}


void NyaBot::addSlash(SlashCommand&& slash){
  slashs.push_back(std::move(slash));
}

void NyaBot::removeSlash(const std::string_view name){
  for(auto it = slashs.begin(); it != slashs.end(); ++it){
    if(it->name == name){
      slashs.erase(it);
      break;
    }
  }
  return;
}

nlohmann::json SlashCommandParameter::generate() const {
  nlohmann::json p;
  p["name"] = name;
  p["description"] = desc;
  p["type"] = type;
  p["required"] = required;
  p["choices"] = nlohmann::json::array();
  for(const auto& choice : choices){
    nlohmann::json c;
    c["name"] = choice;
    c["value"] = choice;
    p["choices"].emplace_back(c);
  }
  return p;
}


nlohmann::json SlashCommand::generate() const {
  nlohmann::json j;
  j["name"] = name;
  j["type"] = 1;
  j["description"] = desc;
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
  j["options"] = nlohmann::json::array();
  for(const auto& param : params){
    j["options"].emplace_back(param.generate());
  }
  return j;
}

std::expected<std::nullopt_t, Error> NyaBot::syncSlashCommands(){
  return syncApplicationCommands();
}

std::expected<std::nullopt_t, Error> NyaBot::syncApplicationCommands(){
  nlohmann::json json;
  json = nlohmann::json::array();
  for(const auto& it : slashs){
    json.emplace_back(it.generate());
  }
  for(const auto& it : ctxMenuCommands){
    json.emplace_back(it.generate());
  }
  auto meow = rest.put(std::format(APIURL "/applications/{}/commands",
                                   api.appId),
                       json.dump());
  if(!meow.has_value() || meow->second != 200){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to sync slash commands");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}
