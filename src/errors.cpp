#include "../include/error.h"

Error::Error(const std::string& str){
  nlohmann::json j;
  try {
    j = nlohmann::json::parse(str);
  } catch(nlohmann::json::exception& e){
    message = str;
    return;
  }
  code = j["code"];
  message = j["message"];
  for(const auto& a : j["errors"].items()){
    if(a.key() != "_errors") continue;
    for(const auto& b : a.value()){
      errors.push_back(std::make_pair(b["code"], b["message"]));
    }
  }
}
