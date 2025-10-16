#ifndef _BUTTON_COMPONENT_H
#define _BUTTON_COMPONENT_H
#include "componentsv2.h"
#include <optional>
#include "../emoji.h"

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
  nlohmann::json generate() override;
  ButtonComponent& setStyle(enum Style);
  ButtonComponent& setCustomId(const std::string_view a);
  ButtonComponent& setLabel(const std::string_view a);
  ButtonComponent& addEmoji(const Emoji& e);
private:
  ComponentTypes type = BUTTON;
  enum Style style{};
  std::optional<std::string> label;
  std::string customId;
  std::optional<Emoji> emoji = std::nullopt;
};
#endif
