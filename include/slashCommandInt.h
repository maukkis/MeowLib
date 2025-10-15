#ifndef _SLASHCOMMAND_H
#define _SLASHCOMMAND_H
#include <unordered_map>
#include "attachment.h"
#include "interaction.h"

class NyaBot;


struct SlashCommandInt : public Interaction {
  using Interaction::Interaction;
  /// A map of parameters key is custom_id value is the value.
  std::unordered_map<std::string, std::string> parameters;
  std::unordered_map<std::string, User> resolvedUsers;
  std::unordered_map<std::string, ResolvedAttachment> resolvedAttachments;
};

#endif 
