#ifndef _INCLUDE_COMPONENTSV2_THUMBNAIL_H
#define _INCLUDE_COMPONENTSV2_THUMBNAIL_H
#include "componentsv2.h"
#include "mediagallery.h"
#include <nlohmann/json_fwd.hpp>

class ThumbnailComponent : public Component{
public:
  nlohmann::json generate() override;
  ThumbnailComponent& addMedia(const MediaItem& m);
private:
  ComponentTypes type = THUMBNAIL;
  MediaItem media = MediaItem("arf");
};


#endif
