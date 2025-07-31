#include "../include/nyaBot.h"

void NyaBot::routeInteraction(ButtonInteraction& interaction){
  if(iCallbacks.buttonInteractionTable.contains(interaction.commandName))
    iCallbacks.buttonInteractionTable[interaction.commandName](interaction);
  else
    funs.onButtonF(interaction);
}


void NyaBot::routeInteraction(SelectInteraction& interaction){
  if(iCallbacks.selectInteractionTable.contains(interaction.commandName))
    iCallbacks.selectInteractionTable[interaction.commandName](interaction);
  else
    funs.onSelectF(interaction);
}


void NyaBot::addInteractionCallback(const std::string_view s,
                                    std::function<void(ButtonInteraction&)> f){
  iCallbacks.buttonInteractionTable.insert({std::string(s), f});
}


void NyaBot::addInteractionCallback(const std::string_view s,
                                    std::function<void(SelectInteraction&)> f){
  iCallbacks.selectInteractionTable.insert({std::string(s), f});
}
