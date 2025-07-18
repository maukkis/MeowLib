#ifndef _INCLUDE_COMPONENTSV2_STRINGSELECTCOMPONENT
#define _INCLUDE_COMPONENTSV2_STRINGSELECTCOMPONENT
#include "componentsv2.h"
#include "selectcomponents.h"
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>


struct StringSelectOption {
  std::string label;
  std::string value;
  std::optional<std::string> description = std::nullopt;
  nlohmann::json serialize();
};

class StringSelectComponent : public SelectComponent, public Component{
public:
  StringSelectComponent& setCustomId(const std::string_view);
  StringSelectComponent& setMinValues(const int);
  StringSelectComponent& setMaxValues(const int);
  StringSelectComponent& setPlaceHolder(const std::string_view);
  nlohmann::json generate() override;
  template<typename... T>
  requires(std::conjunction<std::is_same<T, StringSelectOption>...>::value)
  StringSelectComponent& addOptions(T&&... vals){
    static_assert(sizeof...(vals) <= 25, "cannot have more than 25 options!!");
    (options.emplace_back(std::forward<T>(vals)), ...);
    return *this;
  }
private:
  std::vector<StringSelectOption> options;
  ComponentTypes type = STRING_SELECT;
};

#endif

