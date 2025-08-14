#ifndef _INCLUDE_COMPONENTINTERCATION_H
#define _INCLUDE_COMPONENTINTERCATION_H
#include "interaction.h"
#include "message.h"


class ComponentInteraction : public Interaction {
public:
  using Interaction::Interaction;
  InteractionData interaction;
  void updateMessage();
  void updateMessage(Message& msg);
};

#endif
