#ifndef _INCLUDE_COMPONENTSV2_H
#define _INCLUDE_COMPONENTSV2_H
#include "componentsv2.h"
#include "stringSelect.h"
#include "textInput.h"
#include <type_traits>
#include <memory>


template<typename T>
struct AllowedInLabel : std::false_type {};

template<>
struct AllowedInLabel<StringSelectComponent> : std::true_type {};

template<>
struct AllowedInLabel<TextInputComponent> : std::true_type {};

template<typename T>
concept AllowedInLabel_t = AllowedInLabel<std::remove_reference_t<T>>::value;

class LabelComponent : public Component{
public:
  nlohmann::json generate() override;
  LabelComponent& setLabel(const std::string_view s);
  LabelComponent& setDescription(const std::string_view s);
  template<AllowedInLabel_t T>
  LabelComponent& setComponent(T&& comp){
    component = std::make_shared<std::remove_cvref_t<T>>(std::forward<T>(comp));
    return *this;
  }
private:
  ComponentTypes type = LABEL;
  std::string label;
  std::string description;
  std::shared_ptr<Component> component;
};


#endif
