#include "../../include/nyaBot.h"
#include "../../include/buttonInteraction.h"
#include "../../include/selectInteraction.h"
#include "../../include/modalInteraction.h"
#include <unordered_map>
#include <unordered_set>
#include <string>


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
  
  const std::string commandName = json["data"]["name"];
  SlashCommandInt slash(id, interactionToken, commandName, user, appId, a);

  slash.appPermissions = std::stoull(json["app_permissions"].get<std::string>(), nullptr, 10);
  if(json.contains("guild_id"))
    slash.guildId = json["guild_id"];
  slash.resolvedUsers = deserializeResolved<User>(json["data"]);
  if(json["data"].contains("options")){
    for (const auto& it : json["data"]["options"]){
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

  button.appPermissions = std::stoull(j["app_permissions"].get<std::string>(), nullptr, 10);

  if(j.contains("guild_id"))
    button.guildId = j["guild_id"];
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

  modal.appPermissions = std::stoull(j["app_permissions"].get<std::string>(), nullptr, 10);
  modal.guildId = j["guild_id"];
  modal.resolvedUsers = deserializeResolved<User>(j["data"]);
  for(auto& a : j["data"]["components"]){
    if(!a.contains("component")) continue;
    switch(a["component"]["type"].get<int>()){
      case TEXT_DISPLAY: continue;
      case STRING_SELECT:
      case USER_SELECT:
      case ROLE_SELECT:
      case CHANNEL_SELECT:
      case MENTIONABLE_SELECT:
        {
          const auto& vec = a["component"]["values"].get<std::vector<std::string>>();
          modal.data.insert({a["component"]["custom_id"],
            ModalData{
              .type = static_cast<ComponentTypes>(a["component"]["type"].get<int>()),
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

  select.appPermissions = std::stoull(j["app_permissions"].get<std::string>(), nullptr, 10);
  if(j.contains("guild_id"))
    select.guildId = j["guild_id"];
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
        else if(funs.onSlashF) { // command doesnt have a command handler sending it to the default handler

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
            Log::dbg("not implemented yet\n" + j.dump());
        }
        break;
      }
    case MODAL_SUBMIT:
      {
        auto i = constructModal(j, this);
        routeInteraction(i);
      }
    break;
    default:
      Log::dbg("unknown interaction\n" + j.dump());
    break;
  } 
}
