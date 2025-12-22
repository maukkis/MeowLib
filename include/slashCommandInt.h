#ifndef _SLASHCOMMAND_H
#define _SLASHCOMMAND_H
#include <unordered_map>
#include "attachment.h"
#include "interaction.h"

class NyaBot;


struct SlashCommandInteraction : public Interaction {
  using Interaction::Interaction;
  /// A map of parameters key is custom_id value is the value.
  std::unordered_map<std::string, std::string> parameters;
  std::unordered_map<std::string, User> resolvedUsers;
  std::unordered_map<std::string, ResolvedAttachment> resolvedAttachments;
  std::unordered_map<std::string, Channel> resolvedChannels;
  std::unordered_map<std::string, Role> resolvedRoles;
};

// will be removed in later versions please switch to SlashCommandInteraction
using SlashCommandInt = SlashCommandInteraction;

#endif 
