#ifndef _SLASH_COMMAND_H
#define _SLASH_COMMAND_H
#include <memory>
#include <string>
#include <vector>
#include "commandHandling.h"

enum class Types {
  STRING = 3,
  INTEGER,
  BOOLEAN,
  USER,
  ATTACHMENT = 11,
};

enum class IntegrationTypes {
  GUILD_INSTALL,
  USER_INSTALL,
  BOTH
};

class SlashCommandParameter{
public:
  SlashCommandParameter(const std::string_view name, const std::string_view desc, Types type, bool required);
  SlashCommandParameter& addChoice(const std::string_view choice);
  const std::string name;
  const std::string desc;
  const int type;
  const bool required;
  std::vector<std::string> choices;
};


class SlashCommand {
public:

  SlashCommand(const std::string_view name, const std::string_view desc, IntegrationTypes type);
  SlashCommand& addParam(const SlashCommandParameter& a);
  SlashCommand& operator=(SlashCommand&&) = default;
  SlashCommand(SlashCommand&&) = default;
  template<typename T, typename... Args>
  SlashCommand& withCommandHandler(Args&&... args){
    handler = std::make_unique<T>(std::forward<Args>(args)...);
    return *this;
  }
  std::string name;
  std::string desc;
  std::vector<SlashCommandParameter> params;
  IntegrationTypes types;
  std::unique_ptr<Command> handler = nullptr;
};
#endif
