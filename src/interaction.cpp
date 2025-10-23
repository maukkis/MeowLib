#include "../include/slashCommandInt.h"
#include "../include/log.h"
#include <nlohmann/json.hpp>
#include "../include/nyaBot.h"
#include <format>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <utility>
#include "../include/helpers.h"


Interaction::Interaction(
  const std::string_view id,
  const std::string_view token,
  const std::string_view commandNamee,
  User user,
  const std::string& applicationId,
  NyaBot *bot
) : commandName{commandNamee},
    user{user},
    bot(bot),
    interactionId{id},
    interactionToken{token},
    applicationId(applicationId){}



std::expected<std::nullopt_t, Error> Interaction::respond(const std::string_view response, int flags) {
  nlohmann::json j;
  j["type"] = 4;
  j["data"] = nlohmann::json::object();
  if(flags != 0) j["data"]["flags"] = flags;
  j["data"]["content"] = response;
  return manualResponse(j);
}


std::expected<std::nullopt_t, Error> Interaction::respond(const Message& a){
  if(!a.attachments.empty()){
    nlohmann::json j;
    j["type"] = 4;
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
      Log::error("failed to respond to an interaction");
      err.printErrors();
      return std::unexpected(err);
    }
  } else {
    nlohmann::json b;
    b["type"] = 4;
    b["data"] = a.generate();
    return this->manualResponse(b);
  }
  return std::nullopt;
}



std::expected<std::nullopt_t, Error> Interaction::deleteOriginalResponse(){
  auto res = bot->rest.deletereq(std::format(APIURL "/webhooks/{}/{}/messages/@original",
                                             applicationId, interactionToken));
  if(!res.has_value() || res->second != 204){
      auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
      Log::error("failed to create a follow up message");
      err.printErrors();
      return std::unexpected(err);
  }
  return std::nullopt;
}


std::expected<Message, Error> Interaction::createFollowUpMessage(const std::string_view msg, int flags){
  nlohmann::json j;
  if(flags != 0) j["flags"] = flags;
  j["content"] = msg;
  auto ret = bot->rest.post(std::format(APIURL "/webhooks/{}/{}",
                                        applicationId, interactionToken),
                           j.dump());
  if(!ret.has_value() || ret->second != 200){
      auto err = Error(ret.value_or(std::make_pair("meowHttp IO error", 0)).first);
      Log::error("failed to create a follow up message");
      err.printErrors();
      return std::unexpected(err);
  }
  return Message(nlohmann::json::parse(ret->first));
}



std::expected<Message, Error> Interaction::createFollowUpMessage(const Message& a){
  std::expected<std::pair<std::string, int>, RestErrors> meow;
  if(a.attachments.empty()){
    meow = bot->rest.post(std::format(APIURL "/webhooks/{}/{}",
                                        applicationId, interactionToken),
                          a.generate());
  } else {
    auto payload = makeFormData(a.generate(), "woof", a.attachments);
    meow = bot->rest.sendFormData(std::format(APIURL "/webhooks/{}/{}",
                                              applicationId, interactionToken),
                                  payload,
                                  "woof",
                                  "POST");
  }
  if(!meow.has_value() || meow->second != 200){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to create a follow up message");
    err.printErrors();
    return std::unexpected(err);
  }
  return nlohmann::json::parse(meow->first);
}



std::expected<std::nullopt_t, Error> Interaction::respond(int flags){
  nlohmann::json j;
  j["type"] = 5;
  j["flags"] = flags;
  return manualResponse(j);
}

std::expected<std::nullopt_t, Error> Interaction::respond(const Modal& a){
  nlohmann::json j;
  j["type"] = 9;
  j["data"] = a.generate();
  return manualResponse(j);
}


std::expected<std::nullopt_t, Error> Interaction::manualResponse(const nlohmann::json& j){
  auto meow = bot->rest.post(APIURL "/interactions/" + interactionId + '/' + interactionToken + "/callback",
                            j.dump());
  if(!meow.has_value() || meow->second != 204){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to respond to an interaction");
    err.printErrors();
    return std::unexpected(err);
  }
  return std::nullopt;
}


std::expected<Message, Error> Interaction::manualEdit(const nlohmann::json& j){
    auto meow = bot->rest.patch(APIURL "/webhooks/" + applicationId  + '/' + interactionToken + "/messages/@original",
                   j.dump());
  if(!meow.has_value() || meow->second != 200){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to edit an interaction response");
    err.printErrors();
    return std::unexpected(err);
  }
  return Message(nlohmann::json::parse(meow->first));
}


std::expected<Message, Error> Interaction::edit(const std::string_view response, int flags){
  nlohmann::json j;
  if(flags != 0) j["flags"] = flags;
  j["content"] = response;
  return manualEdit(j);
}


std::expected<Message, Error> Interaction::edit(const Message& a){
  if(a.attachments.empty()){
    return manualEdit(a.generate());
  }
  auto payload = makeFormData(a.generate(), "woof", a.attachments);
  auto meow = bot->rest.sendFormData(std::format(APIURL "/webhooks/{}/{}/messages/@original",
                                                 applicationId, interactionToken),
                                     payload,
                                     "woof",
                                     "PATCH");
  if(!meow.has_value() || meow->second != 200){
    auto err = Error(meow.value_or(std::make_pair("meowHttp IO error", 0)).first);
    Log::error("failed to edit an interaction response");
    err.printErrors();
    return std::unexpected(err);
  }
  return nlohmann::json::parse(meow->first);
}
