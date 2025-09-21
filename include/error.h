#ifndef _INCLUDE_ERROR_H
#define _INCLUDE_ERROR_H
#include <string>
#include <nlohmann/json.hpp>


struct Error {
  Error(const std::string& j);
  int code{};
  std::string message;
  // will be empty for every non request error
  std::vector<std::pair<std::string, std::string>> errors;
};


#endif
