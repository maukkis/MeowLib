#include "../../include/componentsv2/thumbnail.h"


ThumbnailComponent& ThumbnailComponent::addMedia(const MediaItem& m){
  media = m;
  return *this;
}

nlohmann::json ThumbnailComponent::generate(){
  nlohmann::json j = media.generate();
  j["type"] = type;
  return j;
}
