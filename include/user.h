#ifndef _INCLUDE_USER_H
#define _INCLUDE_USER_H
#include <string>
#include <nlohmann/json.hpp>

struct User {
  std::string id;
  std::string avatar;
  std::string discriminator;
  std::string globalName;
  std::string username;
};

User serializeUser(nlohmann::json& j);
#endif
