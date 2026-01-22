#include "componentsv2.h"
#include <string>
#include <optional>
#include <type_traits>

class RadioGroupOption {
public:
  nlohmann::json generate() const;
  RadioGroupOption& setValue(const std::string_view a);
  RadioGroupOption& setLabel(const std::string_view a);
  RadioGroupOption& setDescription(const std::string_view a);
  RadioGroupOption& setDefault(const bool a);
private:
  std::string value;
  std::string label;
  std::optional<std::string> desc;
  std::optional<bool> defaultt;
};

template<typename... T>
concept radioGroupOptions = std::conjunction<std::is_same<RadioGroupOption, std::remove_cvref_t<T>>...>::value;

class RadioGroupActionComponent : public Component {
public:
  nlohmann::json generate() override;
  RadioGroupActionComponent& setCustomId(const std::string_view a);
  RadioGroupActionComponent& setRequired(const bool a);
  template<radioGroupOptions... T>
  RadioGroupActionComponent& addOptions(T&&... opt){
    static_assert(sizeof...(opt) >= 2 && sizeof...(opt) <= 10, "invalid amount of options");
    (options.push_back(std::forward<T>(opt)), ...);
    return *this;
  }
private:
  ComponentTypes type = ComponentTypes::RADIO_GROUP;
  std::string customId;
  bool required = true;
  std::vector<RadioGroupOption> options;
};
