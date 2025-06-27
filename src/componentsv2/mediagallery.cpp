#include "../../include/componentsv2/mediagallery.h"


nlohmann::json MediaGalleryComponent::generate() {
  nlohmann::json j;
  j["type"] = type;
  j["items"] = nlohmann::json::array();
  for(const auto& a : mediaItems){
    j["items"].emplace_back(a.generate());
  }
  return j;
}


Item::Item(std::string a, std::optional<std::string> d) {
  url = std::move(a);
  if(d) desc = std::move(d);
}

nlohmann::json Item::generate() const {
  nlohmann::json j;
  j["media"]["url"] = url;
  if(desc) j["description"] = *desc;
  j["spoiler"] = spoiler ? "true" : "false";
  return j;
}
