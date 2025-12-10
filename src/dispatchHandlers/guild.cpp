#include "../../include/nyaBot.h"
#include <chrono>
#include <mutex>
#include <thread>
#include "../../include/eventCodes.h"
#include "../../include/guild.h"


void NyaBot::guildCreate(nlohmann::json j){
  while(shards.at(0).api.state != GatewayStates::READY)
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  Guild a(j["d"]);
  guild.cache.insertGuildRole(a.id, a.roles);
  guild.cache.insert(a.id, a);
  if(api.unavailableGuildIds.contains(j["d"]["id"])){
    Log::dbg("Guild id: " + j["d"]["id"].get<std::string>() + " meant for caching. Caching the guild and returning");
    std::unique_lock<std::mutex> lock(api.UnavailableGuildIdsmtx);
    api.unavailableGuildIds.erase(j["d"]["id"]);

    return;
  }
  if(funs.onGuildCreateF){
    funs.onGuildCreateF(a);
  }
}

void NyaBot::guildUpdate(nlohmann::json j){
  Guild a(j["d"]);
  guild.cache.insert(a.id, a);
  guild.cache.insertGuildRole(a.id, a.roles);
  if(funs.onGuildUpdateF){
    funs.onGuildUpdateF(a);
  }
}

void NyaBot::guildDelete(nlohmann::json j){
  while(shards.at(0).api.state != GatewayStates::READY){
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  if(api.unavailableGuildIds.contains(j["d"]["id"])){
    std::unique_lock<std::mutex> lock(api.UnavailableGuildIdsmtx);
    api.unavailableGuildIds.erase(j["d"]["id"]);
    return;
  }
  if(funs.onGuildDeleteF){
    UnavailableGuild a = deserializeUnavailableGuild(j["d"]);
    funs.onGuildDeleteF(a);
  }
}

void NyaBot::guildBanAdd(nlohmann::json j){
  if(funs.onGuildBanAddF){
    GuildBan a = deserializeGuildBan(j["d"]);
    user.cache.insert(a.user.id, a.user);
    funs.onGuildBanAddF(a);
  }
}


void NyaBot::guildBanRemove(nlohmann::json j){
  if(funs.onGuildBanRemoveF){
    GuildBan a = deserializeGuildBan(j["d"]);
    user.cache.insert(a.user.id, a.user);
    funs.onGuildBanRemoveF(a);
  }
}

void NyaBot::guildMemberRemove(nlohmann::json j){
  if(funs.onGuildMemberRemoveF){
    User a(j["d"]["user"]);
    user.cache.insert(a.id, a);
    a.guild = GuildUser();
    a.guild->guildId = j["d"]["guild_id"];
    funs.onGuildMemberRemoveF(a);
  }
}


void NyaBot::guildMemberAdd(nlohmann::json j){
  User a(j["d"]["user"]);
  user.cache.insert(a.id, a);
  a.guild = GuildUser(j["d"]);
  guild.cache.insertGuildUser(j["d"]["guild_id"], a);
  a.guild->guildId = j["d"]["guild_id"];
  if(funs.onGuildMemberAddF){
    funs.onGuildMemberAddF(a);
  }
}


void NyaBot::guildMemberUpdate(nlohmann::json j){
  User a(j["d"]["user"]);
  user.cache.insert(a.id, a);
  a.guild = GuildUser(j["d"]);
  guild.cache.insertGuildUser(j["d"]["guild_id"], a);
  a.guild->guildId = j["d"]["guild_id"];
  if(funs.onGuildMemberUpdateF){
    funs.onGuildMemberUpdateF(a);
  }
}

void NyaBot::guildRoleCreate(nlohmann::json j){
  if(funs.onGuildRoleCreateF){
    RoleEvent a
      {
        .guildId = j["d"]["guild_id"], 
        .role = Role(j["d"]["role"])
      };
    guild.cache.insertGuildRole(a.guildId, a.role);
    funs.onGuildRoleCreateF(a);
  }
}


void NyaBot::guildRoleUpdate(nlohmann::json j){
  if(funs.onGuildRoleUpdateF){
    RoleEvent a
      {
        .guildId = j["d"]["guild_id"], 
        .role = Role(j["d"]["role"])
      };
    guild.cache.insertGuildRole(a.guildId, a.role);
    funs.onGuildRoleUpdateF(a);
  }
}

void NyaBot::guildRoleDelete(nlohmann::json j){
  if(funs.onGuildRoleDeleteF){
    RoleDeleteEvent a
      {
        .guildId = j["d"]["guild_id"], 
        .roleId = j["d"]["role_id"]
      };
    guild.cache.removeGuildRole(a.guildId, a.roleId);
    funs.onGuildRoleDeleteF(a);
  }
}

void NyaBot::guildSchedCreate(nlohmann::json j){
  if(funs.onGuildScheduledEventCreateF){
    GuildScheduledEvent a(j["d"]);
    funs.onGuildScheduledEventCreateF(a);
  }
}


void NyaBot::guildSchedUpdate(nlohmann::json j){
  if(funs.onGuildScheduledEventUpdateF){
    GuildScheduledEvent a(j["d"]);
    funs.onGuildScheduledEventUpdateF(a);
  }
}


void NyaBot::guildSchedDelete(nlohmann::json j){
  if(funs.onGuildScheduledEventDeleteF){
    GuildScheduledEvent a(j["d"]);
    funs.onGuildScheduledEventDeleteF(a);
  }
}

void NyaBot::guildSchedUserAdd(nlohmann::json j){
  if(funs.onGuildScheduledEventUserAddF){
    GuildScheduledEventUser a{
      .guildScheduledEventId = j["d"]["guild_scheduled_event_id"],
      .userId = j["d"]["user_id"],
      .guildId = j["d"]["guild_id"],
    };
    funs.onGuildScheduledEventUserAddF(a);
  }
}


void NyaBot::guildSchedUserRemove(nlohmann::json j){
  if(funs.onGuildScheduledEventUserRemoveF){
    GuildScheduledEventUser a{
      .guildScheduledEventId = j["d"]["guild_scheduled_event_id"],
      .userId = j["d"]["user_id"],
      .guildId = j["d"]["guild_id"],
    };
    funs.onGuildScheduledEventUserRemoveF(a);
  }
}


void NyaBot::guildMemberChunk(nlohmann::json j){
  Log::dbg("got a chunk");
  j = j["d"];
  const auto& nonce = j["nonce"];
  if(!guildMembersChunkTable.contains(nonce)){
    Log::error("Unknown chunk received!!!");
    return;
  }
  std::unique_lock<std::mutex> lock(guildMembersChunkTable[nonce].usersmtx);
  for(const auto& a : j["members"]){
    User u(a["user"]);
    user.cache.insert(u.id, u);
    u.guild = GuildUser(a);
    guild.cache.insertGuildUser(j["guild_id"], u);
    guildMembersChunkTable[nonce].users.emplace_back(u);
  }
  lock.unlock();
  int index = j["chunk_index"];
  int chunkCount = j["chunk_count"];
  if(index == chunkCount - 1){
    Log::dbg("received all chunks");
    guildMembersChunkTable[nonce].hp.resume();
    std::unique_lock<std::mutex> lock (guildMemberChunkmtx); 
    guildMembersChunkTable.erase(nonce);
    return;
  }
  Log::dbg("expecting " + std::to_string(chunkCount - index) + " chunks");
}


void NyaBot::rateLimited(nlohmann::json j){
  j = j["d"];
  switch(j["opcode"].get<int>()){
    case 8: [[likely]] {
      const float retryAfter = j["retry_after"];
      const std::string nonce = j["meta"]["nonce"];
      const std::string guildId = j["meta"]["guild_id"];
      Log::error("rate limit on request guild members in guild: " +
                 guildId + " waiting for: "
                 + std::to_string(retryAfter) + " seconds");
      std::this_thread::sleep_for(
        std::chrono::duration<float, std::chrono::seconds::period>(retryAfter));
      Log::dbg("resending payload");
      nlohmann::json a;
      a["op"] = RequestGuildMember;
      a["d"]["guild_id"] = guildId;
      a["d"]["query"] = "";
      a["d"]["limit"] = 0;
      a["d"]["nonce"] = nonce;
      shards.at(0).queue.addToQueue(a.dump());
      break;
    }
    default: [[unlikely]]
      Log::error("rate limit for unknown opcode please make an issue about this as it is unimplemented");
    break;
  }
}
