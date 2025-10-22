#ifndef _INCLUDE_COMPONENTSV2_ROLESELECT_H
#define _INCLUDE_COMPONENTSV2_ROLESELECT_H
#include "componentsv2.h"
#include "selectcomponents.h"
#include <string_view>
#include <type_traits>
#include <vector>




class RoleSelectComponent : public SelectComponent, public Component{
public:
  RoleSelectComponent& setCustomId(const std::string_view);
  RoleSelectComponent& setMinValues(const int);
  RoleSelectComponent& setMaxValues(const int);
  RoleSelectComponent& setPlaceHolder(const std::string_view);
  nlohmann::json generate() override;
  template<typename... T>
  requires(std::conjunction<std::is_same<std::remove_cvref_t<T>, std::string_view>...>::value)
  RoleSelectComponent& addOptions(T... vals){
    static_assert(sizeof...(vals) <= 25, "cannot have more than 25 options!!");
    (options.emplace_back(SelectDefaultValue{.type = "role", .id = vals}), ...);
    return *this;
  }
private:
  std::vector<SelectDefaultValue> options;
  ComponentTypes type = ROLE_SELECT;
};

#endif

