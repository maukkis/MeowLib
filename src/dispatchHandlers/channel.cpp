#include "../../include/nyaBot.h"
#include "../../include/typingstart.h"

TypingStart::TypingStart(const nlohmann::json& j){
  channelId = j["channel_id"];
  if(j.contains("guild_id"))
    guildId = j["guild_id"];
  userId = j["user_id"];
  timestamp = j["timestamp"];
  if(j.contains("member")){
    member = User(j["member"]["user"]);
    member->guild = GuildUser(j["member"]);
  }
}



void NyaBot::channelCreate(nlohmann::json j){
  Channel a(j["d"]);
  channel.cache.insert(a.id, a);
  if(a.guildId)
    guild.cache.insertGuildChannel(a);
  if(funs.onChannelCreateF){
    funs.onChannelCreateF(a);
  }
}

void NyaBot::channelDelete(nlohmann::json j){
  Channel a(j["d"]);
  channel.cache.erase(a.id);
  if(a.guildId){
    if(voiceTaskList.contains(*a.guildId) && voiceTaskList.at(*a.guildId).channelId.value_or("bark") == a.id){
      voiceTaskList.at(*a.guildId).closeCallback(false); 
    }
    guild.cache.removeGuildChannel(*a.guildId, a.id);
  }
  if(funs.onChannelDeleteF){
    funs.onChannelDeleteF(a);
  }
}

void NyaBot::channelUpdate(nlohmann::json j){
  Channel a(j["d"]);
  channel.cache.insert(a.id, a);
  if(a.guildId)
    guild.cache.insertGuildChannel(a);
  if(funs.onChannelUpdateF){
    funs.onChannelUpdateF(a);
  }
}

void NyaBot::typingStart(nlohmann::json j){
  if(funs.onTypingStartF){
    TypingStart a(j["d"]);
    if(a.member){
      user.cache.insert(a.member->id, *a.member);
      guild.cache.insertGuildUser(*a.guildId, *a.member);
    }
    funs.onTypingStartF(a);
  }
}
