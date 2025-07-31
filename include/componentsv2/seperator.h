#ifndef _INCLUDE_COMPONENTSV2_SEPERATOR_H
#define _INCLUDE_COMPONENTSV2_SEPERATOR_H
#include "componentsv2.h"



class SeperatorComponent : public Component {
public:
  nlohmann::json generate() override;
  SeperatorComponent& setSpacing(int spacing);
  SeperatorComponent& showDivider(bool a);
private:
  int spacing = 1;
  bool divider = true;
  ComponentTypes type = SEPERATOR;
};


#endif
