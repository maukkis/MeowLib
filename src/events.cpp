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

void NyaBot::onSelectInteraction(std::function<void(SelectInteraction&)> f){
  funs.onSelectF = f;
}


void NyaBot::onModalSubmit(std::function<void(ModalInteraction&)> f){
  funs.onModalF = f;
}

void NyaBot::onMessageCreate(std::function<void(Message&)> f){
  funs.onMessageCreateF = f;
}

void NyaBot::onMessageUpdate(std::function<void(Message&)> f){
  funs.onMessageUpdateF = f;
}

void NyaBot::onMessageDelete(std::function<void(MessageDelete&)> f){
  funs.onMessageDeleteF = f;
}
