#ifndef _INCLUDE_COMPONENTSV2_SELECTCOMPONENTS
#define _INCLUDE_COMPONENTSV2_SELECTCOMPONENTS
#include "componentsv2.h"
#include <functional>
#include <string_view>

struct SelectDefaultValue{
  std::string id;
  //will be filled in by library
  std::string type = "";
  nlohmann::json serialize(){
    nlohmann::json j;
    j["type"] = type;
    j["id"] = id;
    return j;
  }
};

struct SelectComponent{ 
  std::string customId;
  std::string placeholder;
  int minValues = 1;
  int maxValues = 1;
};


#endif
