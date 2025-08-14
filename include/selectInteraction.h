#ifndef _INCLUDE_SELECTINTERACTION_H
#define _INCLUDE_SELECTINTERACTION_H
#include "componentsv2/componentsv2.h"
#include "interaction.h"
#include <string>
#include <unordered_map>
#include <vector>



class SelectInteraction : public Interaction {
public:
  using Interaction::Interaction;
  std::vector<std::string> values;
  std::unordered_map<std::string, User> resolvedUsers;
  ComponentTypes type;
  InteractionData interaction;
};

#endif
