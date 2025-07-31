#include "../include/nyaBot.h"
#include <string>
#include <type_traits>
#include <unordered_map>
#include "../include/buttonInteraction.h"
#include "../include/selectInteraction.h"


void NyaBot::ready(nlohmann::json j){
  j = j["d"];
  api.sesId = j["session_id"];
  api.resumeUrl = j["resume_gateway_url"];
  j = j["user"];
  api.appId = j["id"];
  std::thread{funs.onReadyF}.detach();
}

void NyaBot::resumed(nlohmann::json j){
  Log::Log("connection resumed! with sequence " + std::to_string(j["s"].get<long>()));
}

namespace {

template<typename T>
std::unordered_map<std::string, T> deserializeResolved(nlohmann::json& d){
  std::unordered_map<std::string, T> map;
  if(std::is_same_v<T, User>){
    for(auto& a : d["resolved"]["users"]){
      map[a["id"]] = deserializeUser(a);
    }
  }
  return map;
}

SlashCommandInt constructSlash(nlohmann::json& json, const std::string& appId){
  const std::string id = json["id"];
  const std::string interactionToken = json["token"];
  User user;
  if(json.contains("member")) user = deserializeUser(json["member"]["user"]);
  else user = deserializeUser(json["user"]);
  json = json["data"];
  const std::string commandName = json["name"];
  SlashCommandInt slash(id, interactionToken, commandName, user, appId);
  slash.resolvedUsers = deserializeResolved<User>(json);
  if(json.find("options") != json.end()){
    for (const auto& it : json["options"]){
      slash.parameters.insert({it["name"], it["value"].get<std::string>()});
    }
  }
  return slash;
}


ButtonInteraction constructButton(nlohmann::json& j){
  const std::string& id = j["id"];
  const std::string& interactionToken = j["token"];
  std::string userId; 
  User user;
  if(j.contains("member")) user = deserializeUser(j["member"]["user"]);
  else user = deserializeUser(j["user"]);
  const std::string& name = j["data"]["custom_id"];
  ButtonInteraction button(id, interactionToken, name, user, j["application_id"]);
  button.id = j["data"]["id"].get<int>();
  return button;
}


SelectInteraction constructSelect(nlohmann::json& j){
  const std::string& id = j["id"];
  const std::string& token = j["token"];
  User user;
  if(j.contains("member")) user = deserializeUser(j["member"]["user"]);
  else user = deserializeUser(j["user"]);
  const std::string& name = j["data"]["custom_id"];
  SelectInteraction select(id, token, name, user, j["application_id"]);
  select.resolvedUsers = deserializeResolved<User>(j["data"]);
  select.type = j["data"]["component_type"];
  for(const auto& a : j["data"]["values"]){
    select.values.emplace_back(a);
  }
  return select;
}

}




void NyaBot::interaction(nlohmann::json j){
  j = j["d"];
  int type = j["type"];
  switch(type){
    case APPLICATION_COMMAND:
      {
        auto slash = constructSlash(j, api.appId);
        if(commands.contains(slash.commandName)){
          commands[slash.commandName]->onCommand(slash);
        }
        else { // command doesnt have a command handler sending it to the default handler
          funs.onSlashF(slash);
        }
        break;
      }
    case MESSAGE_COMPONENT:
      {
        int type = j["data"]["component_type"];
        switch(type){
          case BUTTON:
            {
              auto i = constructButton(j);
              routeInteraction(i);
              break;
            }
          case STRING_SELECT:
          case USER_SELECT:
          case CHANNEL_SELECT:
          case MENTIONABLE_SELECT:
          case ROLE_SELECT:
            {
              auto i = constructSelect(j);
              routeInteraction(i);
              break;
            }
          default:
            Log::Log("not implemented yet\n" + j.dump());
        }
      }
    break;
    default:
      Log::Log("unknown interaction\n" + j.dump());
    break;
  } 
}
