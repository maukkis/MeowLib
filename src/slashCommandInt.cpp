#include "includes/slashCommandInt.h"
#include "../meowHttp/src/includes/https.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <iostream>
#include <nlohmann/json_fwd.hpp>


SlashCommandInt::SlashCommandInt(
  const std::string& id,
  const std::string& token,
  const std::string& commandNamee,
  uint64_t userId
) : commandName{commandNamee},
    userId{userId},
    interactionId{id},
    interactionToken{token} {}

void SlashCommandInt::respond(const std::string& response, bool ephemeral) {
  nlohmann::json j;
  j["type"] = 4;
  j["data"] = nlohmann::json::object();
  if(ephemeral) j["data"]["flags"] = 1 << 6;
  j["data"]["content"] = response;
  auto handle = meowHttp::Https()
    .setUrl("https://discord.com/api/v10/interactions/" + interactionId + '/' + interactionToken + "/callback")
    .setPostfields(j.dump());
  if(handle.perform() != OK) {
    std::cout << "aaaaaaaaaaa\n";
  }
}

