#ifndef _INCLUDE_COMPONENTSV2_CONTAINER_H
#define _INCLUDE_COMPONENTSV2_CONTAINER_H
#include "componentsv2.h"
#include "../helpers.h"
#include <memory>
#include <optional>
#include <type_traits>



template<typename... T>
concept AllowedInContainer = std::conjunction<TopLevelComponent<T>...>::value;


class ContainerComponent : public Component {
public:
  nlohmann::json generate() override;
  template<AllowedInContainer... T>
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
