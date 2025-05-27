#ifndef _BUTTON_COMPONENT_H
#define _BUTTON_COMPONENT_H
#include "componentsv2.h"

enum Style {
  PRIMARY = 1,
  SECONDARY,
  SUCCESS,
  DANGER,
  LINK,
  PREMIUM,
};


class ButtonComponent : public Component {
public:
  nlohmann::json generate() override{ return nlohmann::json();} // placeholder to make the compiler stfu
  ButtonComponent& setStyle(enum Style);
  ButtonComponent& setCustomId(const std::string_view a);
  ButtonComponent& setLabel(const std::string_view a);
private:
  ComponentTypes type = BUTTON;
  enum Style style;
  std::string label;
  std::string customId;
};
#endif
