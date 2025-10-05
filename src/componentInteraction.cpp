#include "../include/componentInteraction.h"
#include "../include/nyaBot.h"
#include "../include/helpers.h"
#include <format>


std::expected<std::nullopt_t, Error> ComponentInteraction::updateMessage(){
  nlohmann::json j;
  j["type"] = 6;
  return manualResponse(j);
}


std::expected<std::nullopt_t, Error> ComponentInteraction::updateMessage(Message& a){
  if(!a.attachments.empty()){
    nlohmann::json j;
    j["type"] = 7;
    j["data"] = a.generate();
    auto payload = makeFormData(j, "woof", a.attachments);
    auto res =
      bot->rest.sendFormData(std::format(APIURL "/interactions/{}/{}/callback",
                                         interactionId,
                                         interactionToken),
                             payload,
                             "woof",
                             "POST"
                             );
    if(!res.has_value() || res->second != 204){
      auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
      Log::error("failed to update an interaction rmessage");
      err.printErrors();
      return std::unexpected(err);
    }
  } else {
    nlohmann::json b;
    b["type"] = 7;
    b["data"] = a.generate();
    return this->manualResponse(b);
  }
  return std::nullopt;
}
