#ifndef _INCLUDE_BUTTONINTERACTION_H
#define _INCLUDE_BUTTONINTERACTION_H


#include "slashCommandInt.h"
class ButtonInteraction : public Interaction {
public:
  using Interaction::Interaction;
  int id;
};


#endif
