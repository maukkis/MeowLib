#include "../../include/nyaBot.h"

void NyaBot::userUpdate(nlohmann::json j){
  User u(j["d"]);
  user.cache.setCurrentUser(u);
  user.cache.insert(u.id, u);
  if(funs.onUserUpdateF){
    funs.onUserUpdateF(u);
  }
}
