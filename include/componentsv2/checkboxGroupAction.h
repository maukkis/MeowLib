#ifndef _INCLUDE_COMPONENTSV2_CHECKBOXGROUPACTION_H
#define _INCLUDE_COMPONENTSV2_CHECKBOXGROUPACTION_H
#include "componentsv2.h"
#include "groupOption.h"
#include <string>
#include <vector>

using CheckboxGroupOption = GroupOption;

class CheckboxGroupActionComponent : public Component {
public:
  nlohmann::json generate() override;
  CheckboxGroupActionComponent& setCustomId(const std::string_view a);
  CheckboxGroupActionComponent& setMinValues(const int a);
  CheckboxGroupActionComponent& setMaxValues(const int a);
  CheckboxGroupActionComponent& setRequired(const bool a);
  template<groupOptions... T>
  CheckboxGroupActionComponent& addOptions(T&&... a){
    static_assert(sizeof...(a) > 0 && sizeof...(a) <= 10, "invalid amount of options");
    (options.emplace_back(std::forward<T>(a)), ...);
    return *this;
  }

private:
  ComponentTypes type = CHECKBOX_GROUP;
  std::string customId;
  std::vector<GroupOption> options;
  std::optional<int> minValues;
  std::optional<int> maxValues;
  std::optional<bool> required;
};

#endif
