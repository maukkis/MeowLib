#ifndef _INCLUDE_COMPONENTSV2_CONTAINER_H
#define _INCLUDE_COMPONENTSV2_CONTAINER_H
#include "componentsv2.h"
#include "actionRowComponent.h"
#include "mediagallery.h"
#include "seperator.h"
#include "textDisplay.h"
#include <memory>
#include <optional>
#include <type_traits>


template<typename T>
struct AllowedInContainer : std::false_type {};

template<>
struct AllowedInContainer<TextDisplayComponent> : std::true_type {};

template<>
struct AllowedInContainer<ActionRowComponent<true>> : std::true_type {};

template<>
struct AllowedInContainer<SeperatorComponent> : std::true_type {};

template<>
struct AllowedInContainer<MediaGalleryComponent> : std::true_type {};


template<typename... T>
concept allowedInContainer = std::conjunction<AllowedInContainer<std::remove_reference_t<T>>...>::value; 


class ContainerComponent : public Component {
public:
  nlohmann::json generate() override;
  template<typename... T>
  requires allowedInContainer<T...>
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
