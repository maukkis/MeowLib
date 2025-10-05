#include "../include/error.h"
#include "../include/log.h"



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
  for(const auto& a : j["errors"].flatten().items()){
    auto key = a.key().substr(0, a.key().rfind("/"));
    std::string type = a.key().substr(a.key().rfind("/")+1);
    if(type == "code"){
      errors[key].first = a.value();
    }
    else if(type == "message"){
      errors[key].second = a.value();
    }
  }
}


void Error::printErrors(){
  Log::error("code: " + std::to_string(code) + " message: " + message);
  for(const auto& a : errors){
    Log::error(a.first + " code: " + a.second.first + " message: " + a.second.second);
  }
}
