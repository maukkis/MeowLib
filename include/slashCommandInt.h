#ifndef _SLASHCOMMAND_H
#define _SLASHCOMMAND_H
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <map>
#include <nlohmann/json.hpp>
#include "message.h"
#include "user.h"



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


class Interaction {
public:
  Interaction(const std::string_view id, const std::string_view token, const std::string_view commandName, User user, const std::string& applicationId);
  ///
  /// @brief Responds to an interaction with a simple message.
  ///
  void respond(const std::string_view response, int flags = 0);
  ///
  /// @brief Responds to an interaction with a defer.
  ///
  void respond();
  /// @brief Responds to an interaction with a Message object.
  /// Reason why you would do this is to be able to attach attachments
  /// or components.
  void respond(const Message& a);
  ///
  /// @brief Responds to an interaction with raw json data
  /// @param j json data
  /// 
  void manualResponse(const nlohmann::json& j);
  ///
  /// @brief Edits an interaction original response with raw json data
  /// @param j json data
  ///
  void manualEdit(const nlohmann::json& j);
  ///
  /// @brief Edits interaction original response with a message.
  ///
  void edit(std::string_view response, int flags = 0);
  /// @brief Edits an interaction original response with a Message object
  /// Reason why you would do this is to be able to attach attachments
  /// or components.
  void edit(const Message& a);
  const std::string commandName;
  const User user;
protected:
  const std::string interactionId;
  const std::string interactionToken;
  const std::string& applicationId;
};

struct SlashCommandInt : public Interaction {
  using Interaction::Interaction;
  /// A map of parameters key is custom_id value is the value.
  std::unordered_map<std::string, std::string> parameters;
  std::unordered_map<std::string, User> resolvedUsers;
};

#endif 
