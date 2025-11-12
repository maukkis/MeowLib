#include "../../include/nyaBot.h"
#include <chrono>
#include <mutex>
#include <thread>
#include "../../include/guild.h"


void NyaBot::guildCreate(nlohmann::json j){
  while(api.state != GatewayStates::READY)
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  Guild a(j["d"]);
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
  if(funs.onGuildUpdateF){
    funs.onGuildUpdateF(a);
  }
}

void NyaBot::guildDelete(nlohmann::json j){
  while(api.state != GatewayStates::READY){
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
  if(funs.onGuiildRoleCreateF){
    RoleEvent a
      {
        .guildId = j["d"]["guild_id"], 
        .role = Role(j["d"]["role"])
      };
    funs.onGuiildRoleCreateF(a);
  }
}


void NyaBot::guildRoleUpdate(nlohmann::json j){
  if(funs.onGuiildRoleUpdateF){
    RoleEvent a
      {
        .guildId = j["d"]["guild_id"], 
        .role = Role(j["d"]["role"])
      };
    funs.onGuiildRoleUpdateF(a);
  }
}

void NyaBot::guildRoleDelete(nlohmann::json j){
  if(funs.onGuiildRoleDeleteF){
    RoleDeleteEvent a
      {
        .guildId = j["d"]["guild_id"], 
        .roleId = j["d"]["role_id"]
      };
    funs.onGuiildRoleDeleteF(a);
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
