#include "../include/attachment.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

ResolvedAttachment::ResolvedAttachment(const nlohmann::json& j){
  id = j["id"];
  filename = j["filename"];
  if(j.contains("title"))
    title = j["title"];

  if(j.contains("description"))
    description = j["description"];

  if(j.contains("content_type"))
    contentType = j["content_type"];

  size = j["size"];
  url = j["url"];
  proxyUrl = j["proxy_url"];
  if(j.contains("height") && !j["height"].is_null())
    height = j["height"];

  if(j.contains("width") && !j["width"].is_null())
    width = j["width"];

  if(j.contains("ephemeral"))
    ephemeral = j["ephemeral"];
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
