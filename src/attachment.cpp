#include "../include/attachment.h"
#include <filesystem>
#include <fstream>
#include <string>


Attachment readFile(const std::string& a){
  std::string name = a.substr(a.rfind("/")+1);
  if(!std::filesystem::exists(a) || std::filesystem::is_directory(a))
    return Attachment();
  std::ifstream b{a}; 
  std::stringstream c;
  c << b.rdbuf();
  return Attachment{
    .data = c.str(),
    .name = a.substr(a.rfind("/")+1)
  };
}
