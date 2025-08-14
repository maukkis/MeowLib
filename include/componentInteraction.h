#ifndef _INCLUDE_COMPONENTINTERCATION_H
#define _INCLUDE_COMPONENTINTERCATION_H
#include "interaction.h"
#include "message.h"


class ComponentInteraction : public Interaction {
public:
  using Interaction::Interaction;
  InteractionData interaction;
  /// @brief responds to a component interaction with DEFERRED_UPDATE_MESSAGE
  void updateMessage();
  /// @brief responds to a component interaction with UPDATE_MESSAGE
  /// @param msg Message object to edit the message to
  void updateMessage(Message& msg);
};

#endif
