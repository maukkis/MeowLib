#ifndef _SLASH_COMMAND_H
#define _SLASH_COMMAND_H
#include <string>
#include <vector>

enum class Types {
  STRING = 3,
  INTEGER,
  BOOLEAN,
  USER,
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
  SlashCommand& addParam(SlashCommandParameter a);
  const std::string name;
  const std::string desc;
  std::vector<SlashCommandParameter> params;
  IntegrationTypes types;
};
#endif
