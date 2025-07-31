#ifndef _INCLUDE_COMPONENTSV2_SECTION_H
#define _INCLUDE_COMPONENTSV2_SECTION_H
#include "componentsv2.h"
#include "textDisplay.h"
#include "buttonComponent.h"
#include "thumbnail.h"
#include <memory>
#include <type_traits>
#include <vector>

template<typename T>
concept accessory = std::is_same<std::remove_cvref_t<T>, ThumbnailComponent>::value 
                    || std::is_same<std::remove_cvref_t<T>, ButtonComponent>::value;               


class SectionComponent : public Component {
public:
  nlohmann::json generate() override;
  template<typename... T>
  requires(std::conjunction<std::is_same<std::remove_cvref_t<T>, TextDisplayComponent>...>::value)
  SectionComponent& addComponents(T&&... c){
    (components.emplace_back(std::forward<T>(c)), ...);
    return *this;
  }
  template<accessory T>
  SectionComponent& addAccessory(T&& a){
    accessory = std::make_shared<std::remove_cvref_t<T>>(std::forward<T>(a));
    return *this;
  }
private:
  ComponentTypes type = SECTION;
  std::vector<TextDisplayComponent> components;
  std::shared_ptr<Component> accessory;
};

#endif
