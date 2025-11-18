#include "../include/presence.h"
#include <nlohmann/json.hpp>
#include "../include/nyaBot.h"
#include "../include/eventCodes.h"


nlohmann::json serializePresence(const Presence& p){
  nlohmann::json j;
  if(p.since)
    j["since"] = *p.since;
  else j["since"] = nullptr;
  if(p.activity)
    j["activities"].emplace_back(serializeActivity(*p.activity));
  j["status"] = p.status;
  j["afk"] = p.afk;
  return j;
}

nlohmann::json serializeActivity(const Activity& a) {
  nlohmann::json j;
  j["name"] = a.name;
  j["type"] = static_cast<int>(a.type);
  if(!a.state.empty())
    j["state"] = a.state;
  if(a.url && static_cast<int>(a.type) == 1)
    j["url"] = *a.url;
  return j;
}

void NyaBot::changePresence(const Presence& p){
  nlohmann::json j;
  j["op"] = eventCodes::PresenceUpdate;
  j["d"] = serializePresence(p);
  queue.addToQueue(j.dump());
}

void NyaBot::setPresence(const Presence& p){
  presence = p;
}
