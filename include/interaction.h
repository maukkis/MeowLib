#ifndef _INCLUDE_INTERACTION_H
#define _INCLUDE_INTERACTION_H
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <map>
#include <nlohmann/json.hpp>
#include "guild.h"
#include "message.h"
#include "modal.h"
#include "user.h"
#include "error.h"
#include <expected>

class NyaBot;


enum MsgFlags {
  SUPPRESS_EMBEDS = 1 << 2,
  EPHEMERAL = 1 << 6,
  SUPPRESS_NOTIFICATIONS = 1 << 12,
};


enum types{
  PING = 1,
  APPLICATION_COMMAND = 2,
  MESSAGE_COMPONENT = 3,
  APPLICATION_COMMAND_AUTOCOMPLETE = 4,
  MODAL_SUBMIT = 5,
};

struct InteractionData{
  std::string id;
  int type;
  std::string name;
};


class Interaction {
public:
  Interaction(const std::string_view id,
              const std::string_view token,
              const std::string_view commandName,
              User user,
              const std::string& applicationId,
              NyaBot *bot);
  ///
  /// @brief Responds to an interaction with a simple message.
  ///
  std::expected<std::nullopt_t, Error> respond(const std::string_view response, int flags = 0);
  ///
  /// @brief Responds to an interaction with a defer.
  ///
  std::expected<std::nullopt_t, Error> respond();
  /// @brief Responds to an interaction with a Message object.
  /// Reason why you would do this is to be able to attach attachments
  /// or components.
  std::expected<std::nullopt_t, Error> respond(const Message& a);
  ///
  /// @brief Responds to an interaction with a modal
  /// @param a Modal object
  ///
  std::expected<std::nullopt_t, Error> respond(const Modal& a);
  ///
  /// @brief Responds to an interaction with raw json data
  /// @param j json data
  /// 
  std::expected<std::nullopt_t, Error> manualResponse(const nlohmann::json& j);
  ///
  /// @brief Edits an interaction original response with raw json data
  /// @param j json data
  ///
  std::expected<std::nullopt_t, Error> manualEdit(const nlohmann::json& j);
  ///
  /// @brief Edits interaction original response with a message.
  ///
  std::expected<std::nullopt_t, Error> edit(std::string_view response, int flags = 0);
  /// @brief Edits an interaction original response with a Message object
  /// Reason why you would do this is to be able to attach attachments
  /// or components.
  std::expected<std::nullopt_t, Error> edit(const Message& a);
  
  /// @brief creates a follow up message to the interaction
  /// @param msg message to send
  /// @param flags optionally message flags
  std::expected<std::nullopt_t, Error> createFollowUpMessage(const std::string_view msg, int flags = 0);
  const std::string commandName;
  const User user;
  std::optional<std::string> guildId;
  NyaBot *bot;
  const std::string interactionId;
  std::string appPermissions;
protected:
  const std::string interactionToken;
  const std::string applicationId;
};

#endif

