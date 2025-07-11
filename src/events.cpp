#include "../include/nyaBot.h"
#include <nlohmann/json.hpp>
#include "../include/slashCommandInt.h"

void NyaBot::onReady(std::function<void ()> f) {
  funs.onReadyF = f;
}

void NyaBot::onSlash(std::function<void (SlashCommandInt&)> f) {
  funs.onSlashF = f;
}

void NyaBot::onAutocomplete(std::function<void ()> f) {
  funs.onAutocompleteF = f;
}


void NyaBot::onButtonPress(std::function<void(ButtonInteraction&)> f){
  funs.onButtonF = f;
}

