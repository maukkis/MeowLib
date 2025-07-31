#ifndef _INCLUDE_COMPONENTSV2_CHANNELSELECT_H
#define _INCLUDE_COMPONENTSV2_CHANNELSELECT_H
#include "componentsv2.h"
#include "selectcomponents.h"
#include <string_view>
#include <type_traits>
#include <vector>




class ChannelSelectComponent : public SelectComponent, public Component{
public:
  ChannelSelectComponent& setCustomId(const std::string_view);
  ChannelSelectComponent& setMinValues(const int);
  ChannelSelectComponent& setMaxValues(const int);
  ChannelSelectComponent& setPlaceHolder(const std::string_view);
  nlohmann::json generate() override;
  template<typename... T>
  requires(std::conjunction<std::is_same<std::remove_cvref_t<T>, std::string_view>...>::value)
  ChannelSelectComponent& addOptions(T... vals){
    static_assert(sizeof...(vals) <= 25, "cannot have more than 25 options!!");
    (options.emplace_back(SelectDefaultValue{.type = "user", .id = vals}), ...);
    return *this;
  }
private:
  std::vector<SelectDefaultValue> options;
  ComponentTypes type = CHANNEL_SELECT;
};

#endif

