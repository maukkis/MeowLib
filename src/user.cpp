#include "../include/user.h"


User deserializeUser(nlohmann::json& j){
  return {
    .id = j["id"],
    .avatar = j["avatar"].is_null() ? "" : j["avatar"],
    .discriminator = j["discriminator"],
    .globalName = j["global_name"].is_null() ? "" : j["global_name"],
    .username = j["username"]
  };
}
