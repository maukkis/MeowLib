#ifndef _INCLUDE_ERROR_H
#define _INCLUDE_ERROR_H
#include <string>
#include <nlohmann/json.hpp>
#include <unordered_map>


struct Error {
  Error(const std::string& j);
  void printErrors();
  int code{};
  std::string message;
  std::unordered_map<std::string, std::pair<std::string, std::string>> errors;
};


#endif
