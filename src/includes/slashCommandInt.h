#ifndef _SLASHCOMMAND_H
#define _SLASHCOMMAND_H
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <map>

enum types{
  PING = 1,
  APPLICATION_COMMAND = 2,
  MESSAGE_COMPONENT = 3,
  APPLICATION_COMMAND_AUTOCOMPLETE = 4,
  MODAL_SUBMIT = 5,
};


class SlashCommandInt {
public:
  SlashCommandInt(const std::string& id, const std::string& token, const std::string& commandName, uint64_t userId);
  void respond(const std::string& response, bool epheramal = false);

  std::unordered_map<std::string, std::string> parameters;
  const std::string commandName;
  const uint64_t userId;
private:
  const std::string interactionId;
  const std::string interactionToken;
};

#endif 
