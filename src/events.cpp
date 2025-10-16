#include "../include/nyaBot.h"
#include <nlohmann/json.hpp>
#include "../include/slashCommandInt.h"



void NyaBot::onAutoModerationRuleCreate(std::function<void(AutoModRule&)> f){
  funs.onAutoModerationRuleCreateF = f;
}


void NyaBot::onAutoModerationRuleUpdate(std::function<void(AutoModRule&)> f){
  funs.onAutoModerationRuleUpdateF = f;
}


void NyaBot::onAutoModerationRuleDelete(std::function<void(AutoModRule&)> f){
  funs.onAutoModerationRuleDeleteF = f;
}


void NyaBot::onAutoModerationActionExecution(std::function<void(AutoModActionExecution&)> f){
  funs.onAutoModerationActionExecutionF = f;
}

void NyaBot::onReady(std::function<void ()> f) {
  funs.onReadyF = f;
}

void NyaBot::onSlash(std::function<void (SlashCommandInteraction&)> f) {
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

void NyaBot::onMessagePollVoteRemove(std::function<void(MessagePollVote&)> f){
  funs.onMessagePollVoteRemoveF = f;
}


void NyaBot::onMessagePollVoteAdd(std::function<void(MessagePollVote&)> f){
  funs.onMessagePollVoteAddF = f;
}


void NyaBot::onMessageReactionAdd(std::function<void(MessageReaction&)> f){
  funs.onMessageReactionAddF = f;
}


void NyaBot::onMessageReactionRemove(std::function<void(MessageReaction&)> f){
  funs.onMessageReactionRemoveF = f;
}

void NyaBot::onGuildCreate(std::function<void(Guild&)> f){
  funs.onGuildCreateF = f;
}

void NyaBot::onGuildUpdate(std::function<void(Guild&)> f){
  funs.onGuildUpdateF = f;
}

void NyaBot::onGuildDelete(std::function<void(UnavailableGuild&)> f){
  funs.onGuildDeleteF = f;
}

void NyaBot::onGuildBanAdd(std::function<void(GuildBan&)> f){
  funs.onGuildBanAddF = f;
}

void NyaBot::onGuildBanRemove(std::function<void(GuildBan&)> f){
  funs.onGuildBanRemoveF = f;
}

void NyaBot::onGuildMemberRemove(std::function<void(User&)> f){
  funs.onGuildMemberRemoveF = f;
}

void NyaBot::onGuildMemberAdd(std::function<void(User&)> f){
  funs.onGuildMemberAddF = f;
}

void NyaBot::onGuildMemberUpdate(std::function<void(User&)> f){
  funs.onGuildMemberUpdateF = f;
}


void NyaBot::onGuildRoleCreate(std::function<void(RoleEvent&)> f){
  funs.onGuiildRoleCreateF = f; 
}

void NyaBot::onGuildRoleUpdate(std::function<void(RoleEvent&)> f){
  funs.onGuiildRoleUpdateF = f; 
}

void NyaBot::onGuildRoleDelete(std::function<void(RoleDeleteEvent&)> f){
  funs.onGuiildRoleDeleteF = f; 
}


void NyaBot::onChannelCreate(std::function<void(Channel&)> f){
  funs.onChannelCreateF = f;
}

void NyaBot::onChannelUpdate(std::function<void(Channel&)> f){
  funs.onChannelUpdateF = f;
}

void NyaBot::onChannelDelete(std::function<void(Channel&)> f){
  funs.onChannelDeleteF = f;
}

void NyaBot::onThreadCreate(std::function<void(Channel&)> f){
  funs.onThreadCreateF = f;
}

void NyaBot::onThreadUpdate(std::function<void(Channel&)> f){
  funs.onThreadUpdateF = f;
}

void NyaBot::onThreadDelete(std::function<void(Channel&)> f){
  funs.onThreadDeleteF = f;
}

void NyaBot::onThreadListSync(std::function<void(ThreadListSync&)> f){
  funs.onThreadListSyncF = f;
}

void NyaBot::onThreadMemberUpdate(std::function<void(ThreadMember&)> f){
  funs.onThreadMemberUpdateF = f;
}

void NyaBot::onThreadMembersUpdate(std::function<void(ThreadMembersUpdate&)> f){
  funs.onThreadMembersUpdateF = f;
}



void NyaBot::onTypingStart(std::function<void(TypingStart&)> f){
  funs.onTypingStartF = f;
}
