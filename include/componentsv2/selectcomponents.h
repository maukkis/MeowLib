#ifndef _INCLUDE_COMPONENTSV2_SELECTCOMPONENTS
#define _INCLUDE_COMPONENTSV2_SELECTCOMPONENTS
#include "componentsv2.h"
#include <functional>
#include <string_view>


struct SelectComponent{ 
  std::string customId;
  std::string placeholder;
  int minValues = 1;
  int maxValues = 1;
};


#endif
