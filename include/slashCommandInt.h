#ifndef _SLASHCOMMAND_H
#define _SLASHCOMMAND_H
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <map>

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


class SlashCommandInt {
public:
  SlashCommandInt(const std::string& id, const std::string& token, const std::string& commandName, uint64_t userId, const std::string& applicationId);
  void respond(const std::string& response, int flags = 0);
  void respond();
  void edit(const std::string& response, int flags = 0);
  std::unordered_map<std::string, std::string> parameters;
  const std::string commandName;
  const uint64_t userId;
private:
  const std::string interactionId;
  const std::string interactionToken;
  const std::string& applicationId;
};

#endif 
