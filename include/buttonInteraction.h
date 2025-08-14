#ifndef _INCLUDE_BUTTONINTERACTION_H
#define _INCLUDE_BUTTONINTERACTION_H
#include "componentInteraction.h"
#include "interaction.h"





class ButtonInteraction : public ComponentInteraction {
public:
  using ComponentInteraction::ComponentInteraction;
  int id;
};


#endif
