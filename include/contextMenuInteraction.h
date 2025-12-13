#ifndef _INCLUDE_CONTEXT_MENU_INTERACTION_H
#define _INCLUDE_CONTEXT_MENU_INTERACTION_H
#include "contextMenuCommand.h"
#include "interaction.h"
#include <unordered_map>

class ContextMenuInteraction : public Interaction {
public:
  using Interaction::Interaction;
  ContextMenuTypes type;
  std::string targetId;
  std::unordered_map<std::string, User> resolvedUsers;
  std::unordered_map<std::string, Message> resolvedMessages;
};

#endif
