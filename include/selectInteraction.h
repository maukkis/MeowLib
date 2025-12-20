#ifndef _INCLUDE_SELECTINTERACTION_H
#define _INCLUDE_SELECTINTERACTION_H
#include "componentInteraction.h"
#include "componentsv2/componentsv2.h"
#include "interaction.h"
#include <unordered_set>
#include <string>
#include <unordered_map>



class SelectInteraction : public ComponentInteraction {
public:
  using ComponentInteraction::ComponentInteraction;
  std::unordered_set<std::string> values;
  std::unordered_map<std::string, User> resolvedUsers;
  ComponentTypes type;
};

#endif
