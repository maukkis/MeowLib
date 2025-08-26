#include "../include/nyaBot.h"
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include "../include/buttonInteraction.h"
#include "../include/selectInteraction.h"
#include "../include/modalInteraction.h"


void NyaBot::ready(nlohmann::json j){
  j = j["d"];
  api.sesId = j["session_id"];
  api.resumeUrl = j["resume_gateway_url"];
  j = j["user"];
  api.appId = j["id"];
  funs.onReadyF();
}

void NyaBot::resumed(nlohmann::json j){
  Log::Log("connection resumed! with sequence " + std::to_string(j["s"].get<long>()));
}

// this is a placeholder do not mind this
void NyaBot::messageCreate([[maybe_unused]]nlohmann::json j){}
void NyaBot::messageUpdate([[maybe_unused]]nlohmann::json j){}
namespace {

InteractionData deserializeInteractionData(nlohmann::json& j){
  return {
    .id = j["message"]["interaction"]["id"],
    .type = j["message"]["interaction"]["type"],
    .name = j["message"]["interaction"]["name"]
  };
}


template<typename T>
std::unordered_map<std::string, T> deserializeResolved(nlohmann::json& d){
  std::unordered_map<std::string, T> map;
  if(std::is_same_v<T, User>){
    for(auto& a : d["resolved"]["users"]){
      map[a["id"]] = deserializeUser(a);
    }
    for(auto it = d["resolved"]["members"].begin(); it != d["resolved"]["members"].end(); ++it){
      map[it.key()].guild = deserializeGuildUser(d["resolved"]["members"][it.key()]);
    }
  }
  return map;
}

SlashCommandInt constructSlash(nlohmann::json& json, const std::string& appId, NyaBot *a){
  const std::string id = json["id"];
  const std::string interactionToken = json["token"];
  User user;
  if(json.contains("member")){
    user = deserializeUser(json["member"]["user"]);
    user.guild = deserializeGuildUser(json["member"]);
  } else user = deserializeUser(json["user"]);
  json = json["data"];
  const std::string commandName = json["name"];
  SlashCommandInt slash(id, interactionToken, commandName, user, appId, a);
  slash.resolvedUsers = deserializeResolved<User>(json);
  if(json.find("options") != json.end()){
    for (const auto& it : json["options"]){
      slash.parameters.insert({it["name"], it["value"].get<std::string>()});
    }
  }
  return slash;
}


ButtonInteraction constructButton(nlohmann::json& j, NyaBot *a){
  const std::string& id = j["id"];
  const std::string& interactionToken = j["token"];
  std::string userId; 
  User user;
  if(j.contains("member")){
    user = deserializeUser(j["member"]["user"]);
    user.guild = deserializeGuildUser(j["member"]);
  } else user = deserializeUser(j["user"]);
  const std::string& name = j["data"]["custom_id"];
  ButtonInteraction button(id, interactionToken, name, user, j["application_id"], a);
  button.interaction = deserializeInteractionData(j);
  button.id = j["data"]["id"].get<int>();
  return button;
}

ModalInteraction constructModal(nlohmann::json& j, NyaBot *a){
  const std::string& id = j["id"];
  const std::string& interactionToken = j["token"];
  std::string userId; 
  User user;
  if(j.contains("member")){
    user = deserializeUser(j["member"]["user"]);
    user.guild = deserializeGuildUser(j["member"]);
  } else user = deserializeUser(j["user"]);
  const std::string& name = j["data"]["custom_id"];
  ModalInteraction modal(id, interactionToken, name, user, j["application_id"], a);
  for(auto& a : j["data"]["components"]){
    if(!a.contains("component")) continue;
    switch(a["component"]["type"].get<int>()){
      case STRING_SELECT:
        {
          const auto& vec = a["component"]["values"].get<std::vector<std::string>>();
          modal.data.insert({a["component"]["custom_id"],
            ModalData{
              .type = STRING_SELECT,
              .data = std::unordered_set<std::string>(vec.begin(), vec.end())}});
          break;
        }
      case TEXT_INPUT:
        modal.data.insert(
          {
            a["component"]["custom_id"],
            ModalData{
              .type = TEXT_INPUT,
              .data = std::string(a["component"]["value"])
            }
          }
        );
    }
  }
  return modal;
}



SelectInteraction constructSelect(nlohmann::json& j, NyaBot *a){
  const std::string& id = j["id"];
  const std::string& token = j["token"];
  User user;
  if(j.contains("member")){
    user = deserializeUser(j["member"]["user"]);
    user.guild = deserializeGuildUser(j["member"]);
  } else user = deserializeUser(j["user"]);
  const std::string& name = j["data"]["custom_id"];
  SelectInteraction select(id, token, name, user, j["application_id"], a);
  select.interaction = deserializeInteractionData(j);
  select.resolvedUsers = deserializeResolved<User>(j["data"]);
  select.type = j["data"]["component_type"];
  for(const auto& a : j["data"]["values"]){
    select.values.emplace(a);
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
        auto slash = constructSlash(j, api.appId, this);
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
              auto i = constructButton(j, this);
              routeInteraction(i);
              break;
            }
          case STRING_SELECT:
          case USER_SELECT:
          case CHANNEL_SELECT:
          case MENTIONABLE_SELECT:
          case ROLE_SELECT:
            {
              auto i = constructSelect(j, this);
              routeInteraction(i);
              break;
            }

          default:
            Log::Log("not implemented yet\n" + j.dump());
        }
      }
    case MODAL_SUBMIT:
      {
        auto i = constructModal(j, this);
        routeInteraction(i);
      }
    break;
    default:
      Log::Log("unknown interaction\n" + j.dump());
    break;
  } 
}
