#ifndef COMPONENTS_V2_H
#define COMPONENTS_V2_H
#include <nlohmann/json_fwd.hpp>

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
  LABEL,
  FILE_UPLOAD = 19,
};



class Component {
public:
  virtual nlohmann::json generate() = 0;
  virtual ~Component() = default;
protected:
  ComponentTypes type;
};

#endif



