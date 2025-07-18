#ifndef _INCLUDE_SELECTINTERACTION_H
#define _INCLUDE_SELECTINTERACTION_H
#include "componentsv2/componentsv2.h"
#include "slashCommandInt.h"
#include <string>
#include <vector>



class SelectInteraction : public Interaction {
public:
  using Interaction::Interaction;
  std::vector<std::string> values;
  ComponentTypes type;
};

#endif
