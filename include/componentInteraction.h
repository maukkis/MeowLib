#ifndef _INCLUDE_COMPONENTINTERCATION_H
#define _INCLUDE_COMPONENTINTERCATION_H
#include "interaction.h"
#include "message.h"


class ComponentInteraction : public Interaction {
public:
  
  ComponentInteraction(const std::string_view id,
              const std::string_view token,
              const std::string_view commandName,
              User user,
              const std::string& applicationId,
              NyaBot *bot, const nlohmann::json& msgj);
  Message message;
  /// @brief responds to a component interaction with DEFERRED_UPDATE_MESSAGE
  std::expected<std::nullopt_t, Error> updateMessage();
  /// @brief responds to a component interaction with UPDATE_MESSAGE
  /// @param msg Message object to edit the message to
  std::expected<std::nullopt_t, Error> updateMessage(Message& msg);
};

#endif
