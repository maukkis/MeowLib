#include "../../include/nyaBot.h"


void NyaBot::messageCreate(nlohmann::json j){
  if(funs.onMessageCreateF){
    Message msg(j["d"]);
    funs.onMessageCreateF(msg);
  }
}


void NyaBot::messageUpdate(nlohmann::json j){
  if(funs.onMessageUpdateF){
    Message msg(j["d"]);
    funs.onMessageUpdateF(msg);
  }
}

void NyaBot::messageDelete(nlohmann::json j){
  if(funs.onMessageDeleteF){
    j = j["d"];
    MessageDelete a
      {
        .id = j["id"],
        .channelId = j["channel_id"],
        .guildId = j.contains("guild_id") ? std::make_optional(j["guild_id"]) : std::nullopt
      };
    funs.onMessageDeleteF(a);
  }
}


void NyaBot::messageReactionAdd(nlohmann::json j){
  if(funs.onMessageReactionAddF){
    MessageReaction a(j["d"]);
    funs.onMessageReactionAddF(a);
  }
}

void NyaBot::messageReactionRemove(nlohmann::json j){
  if(funs.onMessageReactionRemoveF){
    MessageReaction a(j["d"]);
    funs.onMessageReactionRemoveF(a);
  }
}


void NyaBot::messagePollVoteAdd(nlohmann::json j){
  if(funs.onMessagePollVoteAddF){
    MessagePollVote a(j["d"]);
    funs.onMessagePollVoteAddF(a);
  }
}

void NyaBot::messagePollVoteRemove(nlohmann::json j){
  if(funs.onMessagePollVoteRemoveF){
    MessagePollVote a(j["d"]);
    funs.onMessagePollVoteRemoveF(a);
  }
}


