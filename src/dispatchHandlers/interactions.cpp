#include "../../include/nyaBot.h"
#include "../../include/buttonInteraction.h"
#include "../../include/selectInteraction.h"
#include "../../include/modalInteraction.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <type_traits>


namespace {

// TODO: cache these users i am eepy and too lazy to do it

template<typename T>
std::unordered_map<std::string, T> deserializeResolved(const nlohmann::json& d){
  if(!d.contains("resolved")) return std::unordered_map<std::string, T>();
  if constexpr(std::is_same_v<T, User>){
    std::unordered_map<std::string, T> map;
    if(!d["resolved"].contains("users")) return map;
    for(auto& a : d["resolved"]["users"])
      map[a["id"]] = User(a);
    if(d["resolved"].contains("members")){
      for(auto it = d["resolved"]["members"].begin(); it != d["resolved"]["members"].end(); ++it){
        map[it.key()].guild = GuildUser(d["resolved"]["members"][it.key()]);
      }
    }
    return map;
  }
  else if constexpr(std::is_same_v<T, ResolvedAttachment>){
    std::unordered_map<std::string, ResolvedAttachment> map;
    if(!d["resolved"].contains("attachments")) return map;
    for(const auto& a : d["resolved"]["attachments"].items()){
      ResolvedAttachment c(a.value());
      map[a.key()] = std::move(c);
    }
    return map;
  }
  else if constexpr(std::is_same_v<T, Message>){
    std::unordered_map<std::string, Message> map;
    if(!d["resolved"].contains("messages")) return map;
    for(const auto& a : d["resolved"]["messages"].items()){
      Message b(a.value());
      map.insert({a.key(), std::move(b)});
    }
    return map;
  }
}

template<typename T>
concept isInteraction = std::is_base_of_v<Interaction, T>;

template<isInteraction T>
T deserializeInteraction(nlohmann::json& j, NyaBot *a){
  const std::string id = j["id"];
  const std::string interactionToken = j["token"];
  User user;
  if(j.contains("member")){
    user = User(j["member"]["user"]);
    user.guild = GuildUser(j["member"]);
    a->guild.cache.insertGuildUser(j["guild_id"], user);
  } 
  else user = User(j["user"]);
    a->user.cache.insert(user.id, user); 

  std::string name;

  if constexpr(std::is_same_v<T, SlashCommandInteraction> || std::is_same_v<T, ContextMenuInteraction>)
    name = j["data"]["name"];
  else
    name = j["data"]["custom_id"];

  if constexpr(std::is_base_of_v<ComponentInteraction, T>){
    T interaction(id, interactionToken, name, user, j["application_id"], a, j["message"]);
    interaction.appPermissions = std::stoull(j["app_permissions"].get<std::string>(), nullptr, 10);
    if(j.contains("guild_id"))
      interaction.guildId = j["guild_id"];

    if(j.contains("channel_id")) 
      interaction.channelId = j["channel_id"];

    return interaction;
  }

  else {
    T interaction(id, interactionToken, name, user, j["application_id"], a);
    interaction.appPermissions = std::stoull(j["app_permissions"].get<std::string>(), nullptr, 10);
    if(j.contains("guild_id"))
      interaction.guildId = j["guild_id"];

    if(j.contains("channel_id")) 
      interaction.channelId = j["channel_id"];

    return interaction;
  }
}


SlashCommandInteraction constructSlash(nlohmann::json& json, NyaBot *a){
  auto slash = deserializeInteraction<SlashCommandInteraction>(json, a);
  slash.resolvedUsers = deserializeResolved<User>(json["data"]);
  slash.resolvedAttachments = deserializeResolved<ResolvedAttachment>(json["data"]);
  if(json["data"].contains("options")){
    for (const auto& it : json["data"]["options"]){
      slash.parameters.insert({it["name"], it["value"].get<std::string>()});
    }
  }
  return slash;
}


ButtonInteraction constructButton(nlohmann::json& j, NyaBot *a){
  auto button = deserializeInteraction<ButtonInteraction>(j, a);
  button.id = j["data"]["id"].get<int>();
  return button;
}

ModalInteraction constructModal(nlohmann::json& j, NyaBot *a){
  auto modal = deserializeInteraction<ModalInteraction>(j, a);
  modal.resolvedUsers = deserializeResolved<User>(j["data"]);
  modal.resolvedAttachments = deserializeResolved<ResolvedAttachment>(j["data"]);
  for(auto& a : j["data"]["components"]){
    if(!a.contains("component")) continue;
    switch(a["component"]["type"].get<int>()){
      case TEXT_DISPLAY: continue;
      case STRING_SELECT:
      case USER_SELECT:
      case ROLE_SELECT:
      case CHANNEL_SELECT:
      case FILE_UPLOAD:
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
  auto select = deserializeInteraction<SelectInteraction>(j, a);
  select.resolvedUsers = deserializeResolved<User>(j["data"]);
  select.type = j["data"]["component_type"];
  for(const auto& a : j["data"]["values"]){
    select.values.emplace(a);
  }
  return select;
}

ContextMenuInteraction constructCtxMenuInteraction(nlohmann::json& j, NyaBot *a){
  auto interaction = deserializeInteraction<ContextMenuInteraction>(j, a);
  interaction.type = j["data"]["type"];
  interaction.targetId = j["data"]["target_id"];
  interaction.resolvedUsers = deserializeResolved<User>(j["data"]);
  interaction.resolvedMessages = deserializeResolved<Message>(j["data"]);
  return interaction;
}


}




void NyaBot::interaction(nlohmann::json j){
  j = j["d"];
  int type = j["type"];
  switch(type){
    case APPLICATION_COMMAND:
      {
        if(j["data"]["type"] == 1){
          auto slash = constructSlash(j, this);
          if(commands.contains(slash.commandName)){
            commands[slash.commandName]->onCommand(slash);
          }
          else if(funs.onSlashF) { // command doesnt have a command handler sending it to the default handler

            funs.onSlashF(slash);
          }
          
        } else {
          auto ctxmenu = constructCtxMenuInteraction(j, this);
          routeInteraction(ctxmenu);
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
