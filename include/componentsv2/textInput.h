#ifndef _INCLUDE_COMPONENTSV2_TEXTINPUT_H
#define _INCLUDE_COMPONENTSV2_TEXTINPUT_H
#include "componentsv2.h"
#include <string_view>

enum class TextInputStyles {
  SHORT = 1,
  PARAGRAPH,
};


class TextInputComponent : public Component{
public:
  nlohmann::json generate() override;
  TextInputComponent& setCustomId(const std::string_view s);
  TextInputComponent& setStyle(const TextInputStyles s);
  TextInputComponent& setMinimumLength(const int i);
  TextInputComponent& setMaximumLength(const int i);
  TextInputComponent& setRequired(const bool r);
  TextInputComponent& setValue(const std::string_view s);
  TextInputComponent& setPlaceholder(const std::string_view s);
private:
  ComponentTypes type = TEXT_INPUT;
  std::string customId;
  TextInputStyles style;
  std::optional<int> minLen;
  std::optional<int> maxLen;
  bool required = true;
  std::string value;
  std::string placeholder;
};

#endif
