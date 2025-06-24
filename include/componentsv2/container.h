#ifndef _INCLUDE_COMPONENTSV2_CONTAINER_H
#define _INCLUDE_COMPONENTSV2_CONTAINER_H
#include "componentsv2.h"
#include "actionRowComponent.h"
#include "seperator.h"
#include "textDisplay.h"
#include <memory>
#include <optional>
#include <type_traits>


template<typename T>
struct allowedInContainer : std::false_type {};

template<>
struct allowedInContainer<TextDisplayComponent> : std::true_type {};

template<>
struct allowedInContainer<ActionRowComponent<true>> : std::true_type {};

template<>
struct allowedInContainer<SeperatorComponent> : std::true_type {};




class ContainerComponent : public Component {
public:
  nlohmann::json generate() override;
  template<typename... T>
  requires(std::conjunction<allowedInContainer<std::remove_reference_t<T>>...>::value)
  ContainerComponent& addComponents(T&&... comps){
    (components.emplace_back(std::make_shared<std::remove_cvref_t<T>>(std::forward<T>(comps))), ...);
    return *this;
  }
  ContainerComponent& setAccentColor(int color);
  ContainerComponent& setSpoiler(bool val);
private:
  ComponentTypes type = CONTAINER;
  std::vector<std::shared_ptr<Component>> components;
  std::optional<int> accentColor;
  bool spoiler = false; 
};

#endif
