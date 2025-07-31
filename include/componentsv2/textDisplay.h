#ifndef INCLUDE_COMPONENTSV2_TEXTDISPLAY
#define INCLUDE_COMPONENTSV2_TEXTDISPLAY
#include "componentsv2.h"
#include <string_view>


class TextDisplayComponent : public Component {
public:
  nlohmann::json generate() override;
  TextDisplayComponent& setContent(const std::string_view content);
private:
  std::string content;
  ComponentTypes type = TEXT_DISPLAY;
};


#endif

