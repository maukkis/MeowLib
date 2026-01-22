#ifndef _INCLUDE_COMPONENTSV2_GROUPOPTION_H
#define _INCLUDE_COMPONENTSV2_GROUPOPTION_H
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <optional>

class GroupOption {
public:
  nlohmann::json generate() const;
  GroupOption& setValue(const std::string_view a);
  GroupOption& setLabel(const std::string_view a);
  GroupOption& setDescription(const std::string_view a);
  GroupOption& setDefault(const bool a);
private:
  std::string value;
  std::string label;
  std::optional<std::string> desc;
  std::optional<bool> defaultt;
};

template<typename... T>
concept groupOptions = std::conjunction<std::is_same<GroupOption, std::remove_cvref_t<T>>...>::value;


#endif
