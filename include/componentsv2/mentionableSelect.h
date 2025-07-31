#ifndef _INCLUDE_COMPONENTSV2_MENTIONABLESELECT_H
#define _INCLUDE_COMPONENTSV2_MENTIONABLESELECT_H
#include "componentsv2.h"
#include "selectcomponents.h"
#include <string_view>
#include <type_traits>
#include <vector>




class MentionableSelectComponent : public SelectComponent, public Component{
public:
  MentionableSelectComponent& setCustomId(const std::string_view);
  MentionableSelectComponent& setMinValues(const int);
  MentionableSelectComponent& setMaxValues(const int);
  MentionableSelectComponent& setPlaceHolder(const std::string_view);
  nlohmann::json generate() override;
  template<typename... T>
  requires(std::conjunction<std::is_same<std::remove_cvref_t<T>, std::string_view>...>::value)
  MentionableSelectComponent& addOptions(T... vals){
    static_assert(sizeof...(vals) <= 25, "cannot have more than 25 options!!");
    (options.emplace_back(SelectDefaultValue{.type = "user", .id = vals}), ...);
    return *this;
  }
private:
  std::vector<SelectDefaultValue> options;
  ComponentTypes type = MENTIONABLE_SELECT;
};

#endif

