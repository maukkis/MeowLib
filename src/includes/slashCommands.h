#ifndef _SLASH_COMMAND_H
#define _SLASH_COMMAND_H
#include <string>
#include <vector>

enum Types {
  STRING = 3,
  INTEGER,
  BOOLEAN,
  USER,
};


class SlashCommandParameter{
public:
  SlashCommandParameter(const std::string& name, const std::string& desc, Types type, bool required);
  SlashCommandParameter& addChoice(const std::string& choice);
  const std::string name;
  const std::string desc;
  const int type;
  const bool required;
  std::vector<std::string> choices;
};


class SlashCommand {
public:
  SlashCommand(const std::string& name, const std::string& desc);
  SlashCommandParameter& addParam(const std::string& name, const std::string& value, Types type, bool required);
  const std::string name;
  const std::string desc;
  std::vector<SlashCommandParameter> params;
};
#endif
