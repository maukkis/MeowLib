#include "../../include/nyaBot.h"
#include "../../include/voice/voiceconnection.h"
#include "../../include/eventCodes.h"
#include <exception>
#include <meowHttp/websocket.h>
#include <mutex>
#include <unistd.h>

VoiceConnection::VoiceConnection(NyaBot *a) : bot{a} {}

VoiceConnection::~VoiceConnection(){
  api.stop = true;
  if(th.joinable())
    th.join();
  qcv.notify_all();
  if(uth.joinable()) uth.join(); 
  close();
}

void VoiceConnection::flush(){
  std::unique_lock<std::mutex> lock(fmtx);
  fcv.wait(lock, [this]{
    return voiceDataQueue.empty() || api.stop;
  });
}

MeowAsync<void> VoiceConnection::connect(const std::string_view guildId, const std::string_view channelId){
  api.guildId = guildId;
  if(!(bot->api.intents & Intents::GUILD_VOICE_STATES)){
    Log::warn("you do not have GUILD_VOICE_STATES intent enabled please enable it to be able to use voice");
    co_return;
  }
  auto info = co_await getConnectInfo(std::string(guildId), channelId);
  handle.setUrl("https://" + info.endpoint + "/?v=8");
  if(handle.perform() != OK){
    Log::dbg("failed to connect");
    co_return;
  }
  getHello();
  sendIdentify(info);
  th = std::thread(&VoiceConnection::listen, this);
}

void VoiceConnection::disconnect(){
  close();
}

void VoiceConnection::close(){
  // we should... actually stop and join the thread
  api.stop = true;
  qcv.notify_all();
  fcv.notify_all();
  if(th.joinable()) th.join();
  if(uth.joinable()) uth.join();


  sendSilence();

  handle.wsClose(1000, "bye :3");
  ::close(uSockfd);
  nlohmann::json j;
  j["op"] = VoiceStateUpdate;
  nlohmann::json d;
  d["guild_id"] = api.guildId;
  d["channel_id"] = nullptr;
  d["self_mute"] = false;
  d["self_deaf"] = true;
  j["d"] = d;
  if(!bot) return;
  int shard = calculateShardId(api.guildId, bot->getNumShards());
  bot->shards.at(shard).queue.addToQueue(j.dump());
  if(bot->voiceTaskList.contains(api.guildId)){
    std::unique_lock lock(bot->voiceTaskmtx);
    bot->voiceTaskList.erase(api.guildId);
  }

}

void VoiceConnection::sendIdentify(const VoiceInfo& info){
  nlohmann::json j;
  j["op"] = VoiceOpcodes::IDENTIFY;
  nlohmann::json e;
  e["server_id"] = info.guildId;
  e["user_id"] = bot->api.appId;
  e["session_id"] = info.sessionId;
  e["token"] = info.token;
  j["d"] = e;
  handle.wsSend(j.dump(), meowWs::meowWS_TEXT);
}


void VoiceConnection::closer(){
  bot = nullptr;
  close();
}

VoiceTask& VoiceConnection::getConnectInfo(const std::string& guildId, const std::string_view channelId){
  nlohmann::json j;
  j["op"] = VoiceStateUpdate;
  nlohmann::json d;
  d["guild_id"] = guildId;
  d["channel_id"] = channelId;
  d["self_mute"] = false;
  d["self_deaf"] = true;
  j["d"] = d;
  int shard = calculateShardId(guildId, bot->getNumShards());
  std::unique_lock lock(bot->voiceTaskmtx);
  bot->voiceTaskList[guildId] = VoiceTask{};
  bot->voiceTaskList[guildId].closeCallback = std::bind(&VoiceConnection::closer, this);
  lock.unlock();
  bot->shards.at(shard).queue.addToQueue(j.dump());
  return bot->voiceTaskList[guildId];
}

void VoiceConnection::getHello(){
  meowWs::meowWsFrame frame;
  std::string data;
  size_t rlen = handle.wsRecv(data, &frame);
  if(rlen <= 0){
    std::terminate();
  }
  auto j = nlohmann::json::parse(data);
  api.heartbeatInterval = j["d"]["heartbeat_interval"];
}


