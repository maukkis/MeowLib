#include <chrono>
#include <exception>
#include <expected>
#include <meowHttp/websocket.h>
#include <csignal>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "../include/nyaBot.h"
#include <nlohmann/json.hpp>
#include <thread>
#if defined(__GNUC__)
#include <cxxabi.h>
#define demangle(x) abi::__cxa_demangle(x, nullptr, nullptr, nullptr)
#elif defined (_MSC_VER)
#define demangle(x) x
#else
#pragma message "demangling isnt supported"
#define demangle(x) x
#endif



void NyaBot::forceShardCount(int count){
  api.numShards = count;
}



NyaBot::NyaBot(int intents){
  api.intents = intents;
  a = this;
  std::signal(SIGINT, NyaBot::signalHandler);
  #ifndef NDEBUG
    Log::info(std::string("MeowLib version: ") + MEOWLIB_VERSION + " with " + std::to_string(dispatchHandlers.size()) + " gw events implemented");
  #endif
  std::set_terminate([](){
    try {
      if(std::current_exception())
        std::rethrow_exception(std::current_exception());
    }
    catch(std::exception& e){
      Log::error("!!! THIS IS A BUG PLEASE REPORT THIS TO THE DEVELOPERS !!!");
      Log::error("Terminate called after throwing an instance of '" + std::string(demangle(typeid(e).name())) + "'");
      Log::error("  what(): " + std::string(e.what()));
      std::abort();
    }
    Log::error("terminate called without an active exception");
    std::abort();
  });
}


void NyaBot::run(const std::string_view token){
  api.token = token;
  Log::dbg("fetching /gateway/bot :3");
  auto res = rest.get(APIURL "/gateway/bot");
  if(!res.has_value() || res->second != 200){
    if(res->second == 401)
      Log::error("invalid bot token!! *wags tail*");
    Log::error("failed to get /gateway/bot exiting :(");
    return;
  }
  auto j = nlohmann::json::parse(res->first);
  if(j["session_start_limit"]["remaining"] == 0){
    Log::error("no login remaining please attempt after " +
               std::to_string(j["session_start_limit"]["reset_after"].get<int>()) +
               " milliseconds");
    return;
  }
  if(api.numShards == -1){
    api.numShards = j["shards"];
  }
  int maxConc = j["session_start_limit"]["max_concurrency"];
  Log::dbg("max conc: " + std::to_string(maxConc));
  api.defaultUrl = j["url"].get<std::string>().replace(0, 3, "https") + "/?v=10&encoding=json";
  Log::dbg("starting up " + std::to_string(api.numShards) + " shards");
  shards.reserve(api.numShards);
  if(api.numShards == 1){
    shards.emplace_back(this, 0, true);
  } else {
    for(int i = 0; i < api.numShards && !stop; ++i){
      shards.emplace_back(this, i);
      //this is stupid rn but idc
      if((i != 0 && i % maxConc == 0) && !stop){
        Log::dbg("waiting 5 seconds until spinning up a new shard");
        std::this_thread::sleep_for(std::chrono::seconds(5));
      }
    }
  }
  std::unique_lock<std::mutex> lock(runMtx);
  runCv.wait(lock, [this]{
    return stop.load();
  });
}



void NyaBot::close(){
  stop = true;
  for(auto& shard : shards){
    shard.close();
  }
  std::unique_lock<std::mutex> lock(voiceTaskmtx);
  for(const auto& a : voiceTaskList){
    a.second.closeCallback(true);
  }
}



NyaBot::~NyaBot(){
  close();
  Log::dbg("closed!"); 
}








