#include "../include/slashCommandInt.h"
#include "../meowHttp/src/includes/https.h"
#include "../include/log.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <print>
#include <format>
#include <nlohmann/json_fwd.hpp>


Interaction::Interaction(
  const std::string_view id,
  const std::string_view token,
  const std::string_view commandNamee,
  uint64_t userId,
  const std::string& applicationId
) : commandName{commandNamee},
    userId{userId},
    interactionId{id},
    interactionToken{token},
    applicationId(applicationId){}


namespace {


std::pair<std::string, HTTPCODES> sendRawData(const std::string& endpoint,
                 const std::string& method,
                 const std::vector<std::string> headers,
                 const std::string& data) {
  std::string a;
  auto meow = meowHttp::Https()
    .setUrl(endpoint)
    .setWriteData(&a)
    .setPostfields(data)
    .setCustomMethod(method);
  for(const auto& b : headers)
    meow.setHeader(b);
  if(meow.perform() != OK){
    Log::Log("sendRawData failed to endpoint " + endpoint);
  }
  return std::make_pair(a, meow.getLastStatusCode());
}


std::string makeFormData(const nlohmann::json j, const std::string_view boundary, std::vector<Attachment> a){
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
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/interactions/" + interactionId + '/' + interactionToken + "/callback")
    .setHeader("content-type: application/json")
    .setPostfields(j.dump());
  if(handle.perform() != OK) {
    Log::Log("failed to respond to an interaction\n");
  }
}


void Interaction::respond(const Message& a){
  if(!a.attachments.empty()){
    nlohmann::json j;
    j["type"] = 4;
    j["data"] = a.generate();
    auto payload = makeFormData(j, "woof", a.attachments);
    auto res =
      sendRawData(std::format("https://discord.com/api/v10/interactions/{}/{}/callback", interactionId, interactionToken),
                "POST", {"content-type: multipart/form-data;boundary=\"woof\""}, payload);
    if(res.second != 204){
      std::println("{}\n{}", static_cast<int>(res.second), res.first);
    }
  } else {
    nlohmann::json b;
    b["type"] = 4;
    b["data"] = a.generate();
    this->manualResponse(b);
  }
}


void Interaction::respond(){
  nlohmann::json j;
  j["type"] = 5;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/interactions/" + interactionId + '/' + interactionToken + "/callback")
    .setHeader("content-type: application/json")
    .setPostfields(j.dump());
  if(handle.perform() != OK) {
    Log::Log("failed to respond to an interaction");
  }
}


void Interaction::manualResponse(const nlohmann::json& j){
  std::string a;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/interactions/" + interactionId + '/' + interactionToken + "/callback")
    .setHeader("content-type: application/json")
    .setWriteData(&a)
    .setPostfields(j.dump());
  if(handle.perform() != OK || handle.getLastStatusCode() != 204){
    Log::Log("failed to respond to an interaction");
    std::println("{}", a);
    std::fflush(stdout);
  }
}


void Interaction::manualEdit(const nlohmann::json& j){
  std::string a;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/webhooks/" + applicationId  + '/' + interactionToken + "/messages/@original")
    .setHeader("content-type: application/json")
    .setCustomMethod("PATCH")
    .setWriteData(&a)
    .setPostfields(j.dump());
  if(handle.perform() != OK || handle.getLastStatusCode() != 200){
    Log::Log("failed to respond" + a);
  }
}


void Interaction::edit(const std::string_view response, int flags){
  nlohmann::json j;
  if(flags != 0) j["flags"] = flags;
  j["content"] = response;
  std::string a;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/webhooks/" + applicationId  + '/' + interactionToken + "/messages/@original")
    .setCustomMethod("PATCH")
    .setHeader("content-type: application/json")
    .setWriteData(&a)
    .setPostfields(j.dump());
  if(handle.perform() != OK) {
    Log::Log("something went wrong while responding\n" + a);
  }
}


void Interaction::edit(const Message& a){
  this->manualEdit(a.generate());
}
