#include "../include/role.h"

RoleColors::RoleColors(){}

RoleColors::RoleColors(const nlohmann::json& j){
  primaryColor = j["primary_color"];
  if(!j["secondary_color"].is_null())
    secondaryColor = j["secondary_color"];
  if(!j["tertiary_color"].is_null())
    tertiaryColor = j["tertiary_color"];
}

Role::Role(const nlohmann::json& j){
  id = j["id"];
  name = j["name"];
  colours = RoleColors(j["colors"]);
  hoist = j["hoist"];
  if(j.contains("icon") && !j["icon"].is_null())
    icon = j["icon"];
  if(j.contains("unicode_emoji") && !j["icon"].is_null())
    unicodeEmoji = j["unicode_emoji"];
  position = j["position"];
  permissions = j["permissions"];
  managed = j["managed"];
  mentionable = j["mentionable"];
  flags = j["flags"];
}
