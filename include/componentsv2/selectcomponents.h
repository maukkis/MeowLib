#ifndef _INCLUDE_COMPONENTSV2_SELECTCOMPONENTS
#define _INCLUDE_COMPONENTSV2_SELECTCOMPONENTS
#include "componentsv2.h"
#include <string_view>


class SelectComponent{
public:
  auto setCustomId(const std::string_view a) -> decltype(*this);
protected:
  std::string customId;
  std::string placeholder;
  int minValues;
  int maxValues;
};

struct StringSelectOption {
  StringSelectOption(const std::string_view label,
                        const std::string_view value,
                        const std::string_view description);
  std::string label;
  std::string value;
  std::string description;
};
class StringSelectComponent : public SelectComponent, public Component{

};

#endif
