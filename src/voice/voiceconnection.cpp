#include "../../include/nyaBot.h"
#include "../../include/voice/voiceconnection.h"
#include "../../include/eventCodes.h"
#include <cstdint>
#include <exception>
#include <limits>
#include <meowHttp/websocket.h>
#include <mutex>
#include <random>
#include <unistd.h>

VoiceConnection::VoiceConnection(NyaBot *a) : bot{a} {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::uniform_int_distribution<uint32_t> distrib(0, std::numeric_limits<uint32_t>::max());
  api.pNonce = distrib(gen);
}

VoiceConnection::~VoiceConnection(){
  close();
}

void VoiceConnection::flush(){
  std::unique_lock<std::mutex> lock(fmtx);
  fcv.wait(lock, [this]{
    return voiceDataQueue.empty() || api.stop;
  });
  Log::dbg("flush exited");
}

void VoiceConnection::connect(const std::string_view guildId, const std::string_view channelId){
  api.guildId = guildId;
  if(!(bot->api.intents & Intents::GUILD_VOICE_STATES)){
    Log::warn("you do not have GUILD_VOICE_STATES intent enabled please enable it to be able to use voice");
    return;
  }
  auto info = getConnectInfo(std::string(guildId), channelId);
  handle.setUrl("https://" + info.endpoint + "/?v=8");
  if(handle.perform() != OK){
    Log::dbg("failed to connect");
    return;
  }
  api.state = VoiceGatewayState::CONNECTED;
  getHello();
  sendIdentify(info);
  th = std::thread(&VoiceConnection::listen, this);
}

void VoiceConnection::disconnect(){
  close();
}

void VoiceConnection::close(){
  // we should... actually stop and join the thread
  api.state = VoiceGatewayState::DISONNECTED;
  Log::dbg("closing voice :3");
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

void VoiceConnection::reconnect(bool resume, bool waitForNewVoice){
  handle.wsClose(1012, "*bites you*");
  api.state = VoiceGatewayState::DISONNECTED;
  if(waitForNewVoice){
    api.state = VoiceGatewayState::CHANGING_VOICE_SERVER;
    udpInterrupt = true;
    std::unique_lock<std::mutex> lockq(qmtx);
    voiceDataQueue.clear();
    lockq.unlock();
    std::unique_lock lock(voiceServerUpdatemtx);
    Log::dbg("waiting 5 seconds for new voice information");
    voiceServerUpdatecv.wait_for(lock, std::chrono::seconds(5), [this]{
      return api.stop || voiceServerUpdateFlag;
    });
    Log::dbg("voice finished waiting");
    if(api.stop || !voiceServerUpdateFlag){
      api.stop = true;
      return;
    };
    voiceServerUpdateFlag = false;
  }
  handle.setUrl("https://" + voiceinfo.endpoint + "/?v=8");
  if(handle.perform() != OK){
    Log::dbg("failed to connect");
    api.stop = true;
    return;
  }
  api.state = VoiceGatewayState::CONNECTED;
  getHello();
  if(resume){
    nlohmann::json j;
    j["op"] = 7;
    nlohmann::json d;
    d["server_id"] = voiceinfo.guildId;
    d["session_id"] = voiceinfo.sessionId;
    d["token"] = voiceinfo.token;
    d["seq_ack"] = api.seq;
    j["d"] = d;
    handle.wsSend(j.dump(), meowWs::meowWS_TEXT);
    return;
  }
  sendIdentify(voiceinfo);
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


void VoiceConnection::closer(bool forget){
  if(forget)
    bot = nullptr;
  close();
}

VoiceInfo& VoiceConnection::getConnectInfo(const std::string& guildId, const std::string_view channelId){
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
  bot->voiceTaskList[guildId] = VoiceCallbacks{};
  bot->voiceTaskList[guildId].closeCallback = std::bind(&VoiceConnection::closer, this, std::placeholders::_1);
  bot->voiceTaskList[guildId].voiceServerUpdate = std::bind(&VoiceConnection::voiceServerUpdate, this, std::placeholders::_1);
  lock.unlock();
  bot->shards.at(shard).queue.addToQueue(j.dump());
  std::unique_lock<std::mutex> locks(voiceServerUpdatemtx);
  voiceServerUpdatecv.wait(locks, [this]{
    return voiceServerUpdateFlag || api.stop;
  });
  voiceServerUpdateFlag = false;
  return voiceinfo;
}


void VoiceConnection::voiceServerUpdate(VoiceInfo& a){
  std::unique_lock<std::mutex> lock(voiceServerUpdatemtx);
  voiceinfo = a;
  voiceServerUpdateFlag = true;
  a = VoiceInfo{};
  lock.unlock();
  voiceServerUpdatecv.notify_all();
}

void VoiceConnection::getHello(){
  meowWs::meowWsFrame frame;
  std::string data;
  size_t rlen = handle.wsRecv(data, &frame);
  if(rlen <= 0){
    std::terminate();
  }
  Log::dbg("got voice hello");
  auto j = nlohmann::json::parse(data);
  api.heartbeatInterval = j["d"]["heartbeat_interval"];
}


