#include "../include/thread.h"
#include <nlohmann/json.hpp>


ThreadMember::ThreadMember(const nlohmann::json& j){
  if(j.contains("id"))
    id = j["id"];
  if(j.contains("user_id"))
    userId = j["user_id"];
  joinTimestamp = j["join_timestamp"];
  flags = j["flags"];
  if(j.contains("guild_id")){
    guildId = j["guild_id"];
  }
  if(j.contains("member")){
    member = deserializeUser(j["member"]["user"]);
    member->guild = deserializeGuildUser(j["member"]);
  }
}

ThreadListSync::ThreadListSync(const nlohmann::json& j){
  guildId = j["guild_id"];
  if(j.contains("channel_ids")){
    for(const auto& a : j["channel_ids"]){
      channelIds.emplace_back(a);
    }
  }
  for(const auto& a : j["threads"]){
    threads.emplace_back(deserializeChannel(a));
  }
  for(const auto& a : j["members"]){
    members.emplace_back(ThreadMember(a));
  }
}

ThreadMembersUpdate::ThreadMembersUpdate(const nlohmann::json& j){
  id = j["id"];
  guildId = j["guild_id"];
  memberCount = j["member_count"];
  if(j.contains("added_members")){
    for(const auto& a : j["added_members"])
      addedMembers.emplace_back(a);
  }
  if(j.contains("removed_member_ids")){
    for(const auto& a : j["removed_member_ids"])
      removedMemberIds.emplace_back(a);
  }
}
