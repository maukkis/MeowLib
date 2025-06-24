#ifndef _SLASHCOMMAND_H
#define _SLASHCOMMAND_H
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <map>
#include <nlohmann/json.hpp>
#include "message.h"



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
  Interaction(const std::string_view id, const std::string_view token, const std::string_view commandName, uint64_t userId, const std::string& applicationId);
  void respond(const std::string_view response, int flags = 0);
  void respond();
  void respond(const Message& a);
  void manualResponse(const nlohmann::json& j);
  void manualEdit(const nlohmann::json& j);
  void edit(std::string_view response, int flags = 0);
  void edit(const Message& a);
  const std::string commandName;
  const uint64_t userId;
protected:
  const std::string interactionId;
  const std::string interactionToken;
  const std::string& applicationId;
};

class SlashCommandInt : public Interaction {
public:
  using Interaction::Interaction;
  std::unordered_map<std::string, std::string> parameters;
};

#endif 
