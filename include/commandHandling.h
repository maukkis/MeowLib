#ifndef _COMMAND_HANDLING_H
#define _COMMAND_HANDLING_H
#include <string>
#include "slashCommandInt.h"

class Command {
public:
  virtual void onCommand(SlashCommandInteraction& e) = 0;
  virtual ~Command() = default;
private:
};

#endif

