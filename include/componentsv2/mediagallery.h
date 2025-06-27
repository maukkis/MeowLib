#ifndef _INCLUDE_COMPONENTSV2_MEDIAGALLERY_H
#define _INCLUDE_COMPONENTSV2_MEDIAGALLERY_H
#include "componentsv2.h"
#include <optional>
#include <string>
#include <type_traits>


class Item {
public:
  Item(std::string, std::optional<std::string> = std::nullopt);
  Item& setSpoiler(){
    spoiler = true;
    return *this;
  }
  nlohmann::json generate() const;
private:
  bool spoiler = false;
  std::string url;
  std::optional<std::string> desc;
};

template<typename... T>
concept areItems = std::conjunction<std::is_same<std::remove_reference_t<T>, Item>...>::value;

class MediaGalleryComponent : public Component {
public:
  nlohmann::json generate() override;
  template<typename... T>
  requires(areItems<T...>)
  MediaGalleryComponent& addItems(T&&... items){
    static_assert(sizeof...(items) < 10, "cannot have more than 10 items");
    (mediaItems.emplace_back(std::forward<T>(items)), ...);
    return *this;
  }
private:
  std::vector<Item> mediaItems;
  ComponentTypes type = MEDIA_GALLERY;
};



#endif
