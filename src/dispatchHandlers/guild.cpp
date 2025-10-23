#include "../../include/nyaBot.h"
#include <mutex>
#include "../../include/guild.h"


void NyaBot::guildCreate(nlohmann::json j){
  if(funs.onGuildCreateF){
    Guild a = deserializeGuild(j["d"]);
    funs.onGuildCreateF(a);
  }
}

void NyaBot::guildUpdate(nlohmann::json j){
  if(funs.onGuildUpdateF){
    Guild a = deserializeGuild(j["d"]);
    funs.onGuildUpdateF(a);
  }
}

void NyaBot::guildDelete(nlohmann::json j){
  if(funs.onGuildDeleteF){
    UnavailableGuild a = deserializeUnavailableGuild(j["d"]);
    funs.onGuildDeleteF(a);
  }
}

void NyaBot::guildBanAdd(nlohmann::json j){
  if(funs.onGuildBanAddF){
    GuildBan a = deserializeGuildBan(j["d"]);
    funs.onGuildBanAddF(a);
  }
}


void NyaBot::guildBanRemove(nlohmann::json j){
  if(funs.onGuildBanRemoveF){
    GuildBan a = deserializeGuildBan(j["d"]);
    funs.onGuildBanRemoveF(a);
  }
}

void NyaBot::guildMemberRemove(nlohmann::json j){
  if(funs.onGuildMemberRemoveF){
    User a = deserializeUser(j["d"]["user"]);
    a.guild = GuildUser();
    a.guild->guildId = j["d"]["guild_id"];
    funs.onGuildMemberRemoveF(a);
  }
}


void NyaBot::guildMemberAdd(nlohmann::json j){
  if(funs.onGuildMemberAddF){
    User a = deserializeUser(j["d"]["user"]);
    a.guild = deserializeGuildUser(j["d"]);
    a.guild->guildId = j["d"]["guild_id"];
    funs.onGuildMemberAddF(a);
  }

}


void NyaBot::guildMemberUpdate(nlohmann::json j){
  if(funs.onGuildMemberUpdateF){
    User a = deserializeUser(j["d"]["user"]);
    a.guild = deserializeGuildUser(j["d"]);
    a.guild->guildId = j["d"]["guild_id"];
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
  for(const auto& a : j["members"]){
    User u = deserializeUser(a["user"]);
    u.guild = deserializeGuildUser(a);
    guildMembersChunkTable[nonce].users.emplace_back(u);
  }
  int index = j["chunk_index"];
  int chunkCount = j["chunk_count"];
  if(index == (chunkCount-1)){
    Log::dbg("received all chunks");
    guildMembersChunkTable[nonce].hp.resume();
    std::unique_lock<std::mutex> lock (guildMemberChunkmtx); 
    guildMembersChunkTable.erase(nonce);
    return;
  }
  Log::dbg("expecting " + std::to_string(chunkCount - index) + " chunks");
}
