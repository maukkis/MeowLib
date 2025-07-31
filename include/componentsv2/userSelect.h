#ifndef _INCLUDE_COMPONENTSV2_USERSELECT_H
#define _INCLUDE_COMPONENTSV2_USERSELECT_H
#include "componentsv2.h"
#include "selectcomponents.h"
#include <string_view>
#include <type_traits>
#include <vector>




class UserSelectComponent : public SelectComponent, public Component{
public:
  UserSelectComponent& setCustomId(const std::string_view);
  UserSelectComponent& setMinValues(const int);
  UserSelectComponent& setMaxValues(const int);
  UserSelectComponent& setPlaceHolder(const std::string_view);
  nlohmann::json generate() override;
  template<typename... T>
  requires(std::conjunction<std::is_same<std::remove_cvref_t<T>, std::string_view>...>::value)
  UserSelectComponent& addOptions(T... vals){
    static_assert(sizeof...(vals) <= 25, "cannot have more than 25 options!!");
    (options.emplace_back(SelectDefaultValue{.type = "user", .id = vals}), ...);
    return *this;
  }
private:
  std::vector<SelectDefaultValue> options;
  ComponentTypes type = USER_SELECT;
};

#endif

