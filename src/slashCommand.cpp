#include "includes/slashcommand.h"
#include "../meowHttp/src/includes/https.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <nlohmann/json_fwd.hpp>


SlashCommand::SlashCommand(
  const std::string& id,
  const std::string& token,
  const std::string& commandNamee
) : interactionId{id},
    interactionToken{token},
    commandName{commandNamee} {}

void SlashCommand::respond(const std::string& response) {
  nlohmann::json j;
  j["type"] = 4;
  j["data"] = nlohmann::json::object();
  j["data"]["content"] = response;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/interactions/" + interactionId + '/' + interactionToken + "/callback")
    .setPostfields(j.dump());
  if(handle.perform() != OK){
    std::cout << "aaaaaaaaaaa\n";
  }
}


