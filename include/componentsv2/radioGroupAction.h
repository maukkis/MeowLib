#ifndef _INCLUDE_COMPONENTSV2_RADIOGROUPOPTION_H
#define _INCLUDE_COMPONENTSV2_RADIOGROUPOPTION_H
#include "componentsv2.h"
#include <string>
#include "groupOption.h"
#include <optional>
#include <type_traits>

using RadioGroupOption = GroupOption;

class RadioGroupActionComponent : public Component {
public:
  nlohmann::json generate() override;
  RadioGroupActionComponent& setCustomId(const std::string_view a);
  RadioGroupActionComponent& setRequired(const bool a);
  template<groupOptions... T>
  RadioGroupActionComponent& addOptions(T&&... opt){
    static_assert(sizeof...(opt) >= 2 && sizeof...(opt) <= 10, "invalid amount of options");
    (options.push_back(std::forward<T>(opt)), ...);
    return *this;
  }
private:
  ComponentTypes type = ComponentTypes::RADIO_GROUP;
  std::string customId;
  bool required = true;
  std::vector<GroupOption> options;
};


#endif
