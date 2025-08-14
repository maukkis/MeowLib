#include "../include/slashCommandInt.h"
#include "../include/log.h"
#include <nlohmann/json.hpp>
#include "../include/nyaBot.h"
#include <format>
#include <nlohmann/json_fwd.hpp>
#include <utility>


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


namespace {


std::string makeFormData(const nlohmann::json j, const std::string_view boundary, const std::vector<Attachment>& a){
  std::string data = std::format("--{}\r\n", boundary);
  data.append("Content-Disposition: form-data; name=\"payload_json\"\r\nContent-Type: application/json\r\n\r\n");
  data.append(j.dump());
  for(size_t i = 0; i < a.size(); ++i){
    data.append(std::format("\r\n--{}\r\n", boundary));
    data.append(std::format("Content-Disposition: form-data; name=\"files[{}]\"; filename=\"{}\"\r\n\r\n", i , a.at(i).name));
    data.append(a.at(i).data);
  }
  data.append(std::format("\r\n--{}--", boundary));
  return data;
}


}


void Interaction::respond(const std::string_view response, int flags) {
  nlohmann::json j;
  j["type"] = 4;
  j["data"] = nlohmann::json::object();
  if(flags != 0) j["data"]["flags"] = flags;
  j["data"]["content"] = response;
  manualResponse(j);
}


void Interaction::respond(const Message& a){
  if(!a.attachments.empty()){
    nlohmann::json j;
    j["type"] = 4;
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
      Log::Log("failed to respond to an interaction\n" + res.value_or(std::make_pair("", 0)).first);
    }
  } else {
    nlohmann::json b;
    b["type"] = 4;
    b["data"] = a.generate();
    this->manualResponse(b);
  }
}

void Interaction::createFollowUpMessage(const std::string_view msg, int flags){
  nlohmann::json j;
  if(flags != 0) j["flags"] = flags;
  j["content"] = msg;
  auto ret = bot->rest.post(std::format("https://discord.com/api/v10/webhooks/{}/{}",
                                        applicationId, interactionToken),
                           j.dump());
  if(!ret.has_value() || ret->second != 200){
    Log::Log("failed to create a follow up message" +
             ret.value_or(std::make_pair("", 0)).first);
  }
}

void Interaction::respond(){
  nlohmann::json j;
  j["type"] = 5;
  manualResponse(j);
}


void Interaction::manualResponse(const nlohmann::json& j){
  auto meow = bot->rest.post("https://discord.com/api/v10/interactions/" + interactionId + '/' + interactionToken + "/callback",
                            j.dump());
  if(!meow.has_value() || meow->second != 204){
    Log::Log("failed to respond to an interaction\n" + meow.value_or(std::make_pair("", 0)).first);
  }
}


void Interaction::manualEdit(const nlohmann::json& j){
    auto meow = bot->rest.patch("https://discord.com/api/v10/webhooks/" + applicationId  + '/' + interactionToken + "/messages/@original",
                   j.dump());
  if(!meow.has_value() || meow->second != 200){
    Log::Log("failed to edit an interaction\n" + meow.value_or(std::make_pair("", 0)).first);
  }
}


void Interaction::edit(const std::string_view response, int flags){
  nlohmann::json j;
  if(flags != 0) j["flags"] = flags;
  j["content"] = response;
  manualEdit(j);
}


void Interaction::edit(const Message& a){
  if(a.attachments.empty()){
    manualResponse(a.generate());
    return;
  }
  auto payload = makeFormData(a.generate(), "woof", a.attachments);
  auto meow = bot->rest.sendFormData(std::format("https://discord.com/api/v10/webhooks/{}/{}/messages/@original",
                                                 applicationId, interactionToken),
                                     payload,
                                     "woof",
                                     "PATCH");
  if(!meow.has_value() || meow->second != 200)
    Log::Log("failed to edit an interaction\n" +
             meow.value_or(std::make_pair("", 0)).first);
}
