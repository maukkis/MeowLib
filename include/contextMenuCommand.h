#ifndef _INCLUDE_CONTEXT_MENU_COMMAND_H
#define _INCLUDE_CONTEXT_MENU_COMMAND_H
#include "slashCommands.h"
#include <nlohmann/json_fwd.hpp>
#include <string>


enum class ContextMenuTypes {
  USER = 2,
  MESSAGE = 3,
};


class ContextMenuCommand {
public:
  ContextMenuCommand(const std::string_view name, ContextMenuTypes type, IntegrationTypes types);
  nlohmann::json generate() const;
private:
  std::string name;
  ContextMenuTypes type;
  IntegrationTypes types;
};



#endif
