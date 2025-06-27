#include "../include/attachment.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>

Attachment::Attachment(const std::string& a) {
  name = a.substr(a.rfind("/")+1);
  if(!std::filesystem::exists(a) || std::filesystem::is_directory(a))
    throw(std::runtime_error("file doesnt exist or is a directory idiot"));
  std::ifstream b{a}; 
  std::stringstream c;
  c << b.rdbuf();
  data = c.str();
}


Attachment readFile(const std::string& a){
  return Attachment(a);
}
