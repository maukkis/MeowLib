#ifndef _INCLUDE_MODAL_H
#define _INCLUDE_MODAL_H
#include "componentsv2/componentsv2.h"
#include "componentsv2/label.h"
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <type_traits>


class Modal {
public:
  nlohmann::json generate() const;
  Modal& setCustomId(const std::string_view s);
  Modal& setTitle(const std::string_view s);
  template<typename... T>
  requires(std::conjunction<std::is_same<std::remove_cvref_t<T>, LabelComponent>...>::value)
  Modal& addComponents(T&&... comps){
    static_assert(sizeof...(comps) <= 5, "cannot have more than 5 components");
    (components.emplace_back(std::make_unique<std::remove_cvref_t<T>>(std::forward<T>(comps))), ...);
    return *this;
  }
private:
  std::string customId;
  std::string title;
  std::vector<std::unique_ptr<Component>> components;
};

#endif
