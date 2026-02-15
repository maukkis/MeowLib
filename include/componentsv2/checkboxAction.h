#ifndef _INCLUDE_COMPONENTSV2_CHECKBOXACTION_H
#define _INCLUDE_COMPONENTSV2_CHECKBOXACTION_H
#include "componentsv2.h"
#include <string>
#include <optional>

class CheckboxActionComponent : public Component {
public:
  nlohmann::json generate() override;
  CheckboxActionComponent& setCustomId(const std::string_view a);
  CheckboxActionComponent& setDefault(const bool a);
private:
  ComponentTypes type = CHECKBOX;
  std::string customId;
  std::optional<bool> defaultt;
};

#endif
