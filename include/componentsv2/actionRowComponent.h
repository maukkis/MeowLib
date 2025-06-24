#ifndef _ACTION_ROW_COMPONENT_H
#define _ACTION_ROW_COMPONENT_H

#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <memory>
#include "componentsv2.h"
#include "buttonComponent.h"
#include "selectcomponents.h"
#include <optional>
// im sorry for whoever is having to edit this --Luna

template<typename T>
using buttons = std::is_same<std::remove_reference_t<T>, ButtonComponent>;



template<bool state = false>
class ActionRowComponent : public Component{
public:
  nlohmann::json generate() override;
  ActionRowComponent() = default;
  template<typename... E>
  requires (std::conjunction<buttons<E>...>::value)
  ActionRowComponent<true> addComponent(E&&... comps){
    static_assert(state != true, "this component can only have 1 type of component");
    static_assert(sizeof...(comps) <= 5, "cannot have more than 5 buttons");
    ActionRowComponent<true> arf;
    (arf.components.push_back(std::make_unique<ButtonComponent>(std::forward<E>(comps))), ...);
    return arf;
  }


  template<typename E>
  requires (std::is_base_of<SelectComponent, E>::value)
  ActionRowComponent<true> addComponent(E&& comps){
    static_assert(state != true, "this component can only have 1 type of component");
    ActionRowComponent<true> arf;
    arf.components.push_back(std::make_unique<SelectComponent>(std::forward<E>(comps)));
    return arf;
  }
  
  std::vector<std::unique_ptr<Component>> components;
private:
  ComponentTypes type = ACTION_ROW;
};

#endif

