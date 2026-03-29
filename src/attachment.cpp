#include "../include/attachment.h"
#include "../include/helpers.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>


ResolvedAttachment::ResolvedAttachment(const nlohmann::json& j){
  try {
    id = j["id"];
    filename = j["filename"];
    jsonToOptional(title, j, "title");
    
    jsonToOptional(description, j, "description");

    jsonToOptional(contentType, j, "content_type");

    size = j["size"];
    url = j["url"];
    proxyUrl = j["proxy_url"];
    jsonToOptional(height, j, "height");

    jsonToOptional(width, j, "width");

    jsonToOptional(ephemeral, j, "ephemeral");
  } catch(nlohmann::json::exception& e){
    Log::error("please check this potential attachment bug\n" + std::string(e.what()) + "\n" + j.dump());
  }
}

Attachment readFile(const std::string& a){
  std::string name = a.substr(a.rfind("/")+1);
  if(!std::filesystem::exists(a) || std::filesystem::is_directory(a))
    return Attachment();
  std::ifstream b(a, std::ios::binary); 
  std::ostringstream c;
  c << b.rdbuf();
  return Attachment{
    .data = c.str(),
    .name = name
  };
}
