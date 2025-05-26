#ifndef COMPONENTS_V2_H
#define COMPONENTS_V2_H
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <memory>
#include <optional>
// im sorry for whoever is having to edit this --Luna


enum ComponentTypes {
  ACTION_ROW = 1,
  BUTTON,
  STRING_SELECT,
  TEXT_INPUT,
  USER_SELECT,
  ROLE_SELECT,
  MENTIONABLE_SELECT,
  CHANNEL_SELECT,
  SECTION,
  TEXT_DISPLAY,
  THUMBNAIL,
  MEDIA_GALLERY,
  FILE_TYPE,
  SEPERATOR,
  CONTAINER = 17,
};



class Component {
public:
  virtual nlohmann::json generate() = 0;
  virtual ~Component() = default;
protected:
  ComponentTypes type;
};

class ButtonComponent : public Component {
public:
  nlohmann::json generate() override{ return nlohmann::json();}

};

class SelectComponent : public Component {
  nlohmann::json generate() override{ return nlohmann::json();}
};

template<bool state = false>
class ActionRowComponent : public Component{
public:
  nlohmann::json generate() override{return nlohmann::json();};
  int bark(){ return components.size();}

  template<typename... E>
  requires (std::conjunction<std::is_same<E, ButtonComponent>...>::value)
  ActionRowComponent<true> addComponent(E&&... comps){
    static_assert(state != true, "this component can only have 1 type of component");
    static_assert(sizeof...(comps) <= 5, "cannot have more than 5 buttons");
    ActionRowComponent<true> arf;
    (arf.components.push_back(std::make_unique<ButtonComponent>(std::forward<E>(comps))), ...);
    return arf;
  }


  template<typename... E>
  requires (std::conjunction<std::is_same<E, SelectComponent>...>::value)
  ActionRowComponent<true> addComponent(E&&... comps){
    static_assert(sizeof...(comps) <= 1, "cannot have more than 1 SelectComponent");
    static_assert(state != true, "this component can only have 1 type of component");
    ActionRowComponent<true> arf;
    (arf.components.push_back(std::make_unique<SelectComponent>(std::forward<E>(comps))), ...);
    return arf;
  }

  std::vector<std::unique_ptr<Component>> components;
private:
  ComponentTypes type = ACTION_ROW;
};

#endif

