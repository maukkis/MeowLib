#ifndef _INCLUDE_COMPONENTSV2_STRINGSELECTCOMPONENT
#define _INCLUDE_COMPONENTSV2_STRINGSELECTCOMPONENT
#include "componentsv2.h"
#include "selectcomponents.h"
#include "../emoji.h"
#include <optional>
#include <string_view>
#include <string>
#include <type_traits>
#include <vector>


struct StringSelectOption {
  std::string label;
  std::string value;
  std::optional<std::string> description = std::nullopt;
  std::optional<Emoji> emoji = std::nullopt;
  nlohmann::json serialize();
};

template<typename... T>
concept StringSelectOptions = std::conjunction<std::is_same<std::remove_reference_t<T>, StringSelectOption>...>::value;


class StringSelectComponent : public SelectComponent, public Component{
public:
  StringSelectComponent& setCustomId(const std::string_view);
  StringSelectComponent& setMinValues(const int);
  StringSelectComponent& setMaxValues(const int);
  StringSelectComponent& setPlaceHolder(const std::string_view);
  // ONLY FOR MODALS
  StringSelectComponent& setRequired(const bool);
  nlohmann::json generate() override;
  template<StringSelectOptions... T>
  StringSelectComponent& addOptions(T&&... vals){
    static_assert(sizeof...(vals) <= 25, "cannot have more than 25 options!!");
    (options.emplace_back(std::forward<T>(vals)), ...);
    return *this;
  }
private:
  std::vector<StringSelectOption> options;
  ComponentTypes type = STRING_SELECT;
  std::optional<bool> required;
};

#endif

