#ifndef _ACTION_ROW_COMPONENT_H
#define _ACTION_ROW_COMPONENT_H

#include <nlohmann/json.hpp>
#include <type_traits>
#include <vector>
#include <memory>
#include "componentsv2.h"
#include "buttonComponent.h"
#include "selectcomponents.h"
// im sorry for whoever is having to edit this --Luna

template<typename... T>
concept buttons = std::conjunction_v<std::is_same<std::remove_reference_t<T>, ButtonComponent>...>;

enum ActionRowComponentStates {
  NONE,
  BUTTONS,
  SELECT,
};

template<ActionRowComponentStates state = NONE>
class ActionRowComponent : public Component{
public:

  nlohmann::json generate() override{
    // we cant really check if its empty at compile time :( unless i do stateful metaprogramming which.... isnt really conformant
    if(state == NONE)
      Log::error("you didnt set child components to action row this will error");
    nlohmann::json j;
    j["type"] = type;
    j["components"] = nlohmann::json::array();
    for (const auto& a : components){
      j["components"].emplace_back(a->generate());
    }
    return j;
  }
  ActionRowComponent() = default;
  template<buttons... E>
  ActionRowComponent<BUTTONS> addComponents(E&&... comps){
    static_assert(state == NONE || state == BUTTONS, "this component can only have 1 type of component");
    static_assert(sizeof...(comps) <= 5, "cannot have more than 5 buttons");
    ActionRowComponent<BUTTONS> arf;
    (arf.components.push_back(std::make_unique<ButtonComponent>(std::forward<E>(comps))), ...);
    return arf;
  }


  template<Select E>
  ActionRowComponent<SELECT> addComponents(E&& comps){
    static_assert(state == NONE || state == SELECT, "this component can only have 1 type of component");
    ActionRowComponent<SELECT> arf;
    arf.components.push_back(std::make_unique<std::remove_cvref_t<E>>(std::forward<E>(comps)));
    return arf;
  }
  std::vector<std::unique_ptr<Component>> components;
private:
  ComponentTypes type = ACTION_ROW;
};

#endif

