#include "../include/componentInteraction.h"
#include "../include/nyaBot.h"

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
