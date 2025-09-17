#include "../include/componentInteraction.h"
#include "../include/nyaBot.h"
#include "../include/helpers.h"


void ComponentInteraction::updateMessage(){
  nlohmann::json j;
  j["type"] = 6;
  manualResponse(j);
}


void ComponentInteraction::updateMessage(Message& a){
  if(!a.attachments.empty()){
    nlohmann::json j;
    j["type"] = 7;
    j["data"] = a.generate();
    auto payload = makeFormData(j, "woof", a.attachments);
    auto res =
      bot->rest.sendFormData(std::format("https://discord.com/api/v10/interactions/{}/{}/callback",
                                         interactionId,
                                         interactionToken),
                             payload,
                             "woof",
                             "POST"
                             );
    if(!res.has_value() || res->second != 204){
      Log::error("failed to respond to an interaction\n" + res.value_or(std::make_pair("", 0)).first);
    }
  } else {
    nlohmann::json b;
    b["type"] = 7;
    b["data"] = a.generate();
    this->manualResponse(b);
  }

}
