#ifndef _INCLUDE_COMPONENTINTERCATION_H
#define _INCLUDE_COMPONENTINTERCATION_H
#include "interaction.h"
#include "message.h"


class ComponentInteraction : public Interaction {
public:
  using Interaction::Interaction;
  InteractionData interaction;
  /// @brief responds to a component interaction with DEFERRED_UPDATE_MESSAGE
  std::expected<std::nullopt_t, Error> updateMessage();
  /// @brief responds to a component interaction with UPDATE_MESSAGE
  /// @param msg Message object to edit the message to
  std::expected<std::nullopt_t, Error> updateMessage(Message& msg);
};

#endif
