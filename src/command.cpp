/* nyaBot simple discord bot written in C++ using libcurl
    Copyright (C) 2024  Luna
This file is part of nyaBot.

nyaBot is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

nyaBot is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with nyaBot; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */


#include "includes/nyaBot.h"
#include <cstdlib>
#include <cstring>
#include "includes/getAutoCorrect.h"
#include "includes/calculatePP.h"
#include "../meowHttp/src/includes/https.h"

enum types{
  PING = 1,
  APPLICATION_COMMAND = 2,
  MESSAGE_COMPONENT = 3,
  APPLICATION_COMMAND_AUTOCOMPLETE = 4,
  MODAL_SUBMIT = 5,
};

static std::string doubleTo2Precision(double meow){
  std::ostringstream bark;
  bark << std::fixed << std::setprecision(2) << meow;
  return bark.str();
}

static void sendResponse(const std::string& json, meowHttp::Https &meow, const std::string& interactionId, const std::string& interactionToken){
    meow.setUrl("https://discord.com/api/interactions/" + interactionId + '/' + interactionToken + "/callback")
      .setPostfields(json)
      .perform();
}

void nyaBot::handleSlash(nlohmann::json meowJson){
  auto meow = meowHttp::Https();
  meowJson = meowJson["d"];
  std::string interactionId = meowJson["id"];
  std::string interactionToken = meowJson["token"];
  auto dataJson = meowJson["data"];
  std::string commandName = dataJson["name"];
  int type = meowJson["type"];
  switch(type){
  case APPLICATION_COMMAND:
    if (commandName == "meow"){
      std::cout << "[*] got slash meow\n";
      std::string json{R"({"type":4, "data":{"content": "meow"}})"};
      sendResponse(json, meow, interactionId, interactionToken);
    }
    if (commandName == "showpp"){
      dataJson = dataJson["options"];
      std::string value;
      for(const auto& tapamut : dataJson){
        value = tapamut["value"].get<std::string>();
      }
      std::string woof;
      meow.setUrl("https://scoresaber.com/api/player/" + value + "/basic")
      .setWriteData(&woof)
      .perform();
      auto nyaJson = nlohmann::json::parse(woof);
      size_t pp = nyaJson["pp"];
      std::string ppString{"8"};
      for(size_t i{0}; i < pp / 100; ++i ){
        ppString.append("="); 
      }
      ppString.append("D");
      std::string json{R"({"type":4, "data":{"content": "pp size is )" + ppString + R"("}})"};
      sendResponse(json, meow, interactionId, interactionToken);
      }
      if(strcmp(commandName.c_str(), "calculatepp") == 0){
        std::string song;
        std::string instrument;
        int score;
        dataJson = dataJson["options"];
        for(const auto& nya : dataJson){
          if(nya["name"] == "song"){
            song = nya["value"].get<std::string>();
          }
          else if(nya["name"] == "instrument"){
            instrument = nya["value"].get<std::string>();
          }
          else if(nya["name"] == "score"){
            score = nya["value"].get<int>();
          }
        }
        std::string json{R"({"type":4, "data":{"content": "pp is )" + doubleTo2Precision(getPP(song, instrument, score)) + R"("}})"};
        std::cout << "woofs at you\n"; 
        sendResponse(json, meow, interactionId, interactionToken);
      }
    break;
    case APPLICATION_COMMAND_AUTOCOMPLETE:
      std::cout << "[*] got autocorrect!\n";
      nlohmann::json j;
      j["type"] = 8;
      j["data"] = nlohmann::json::object();
      j["data"]["choices"] = nlohmann::json::array();
      std::string value;
      dataJson = dataJson["options"];
      for(const auto& tapamut: dataJson){
        if(tapamut["name"] == "song"){
          value = tapamut["value"].get<std::string>();
        }
      }
      std::vector<std::string> bark{getArrayOfOptions(value)};
      if (bark.size() > 25){
        bark.resize(25);
      }
      for(const auto c: bark){
        nlohmann::json opt;
        opt["name"] = c;
        opt["value"] = c;
        j["data"]["choices"].push_back(opt); 
      }
      size_t sent;
      std::string json = j.dump();
      sendResponse(json, meow, interactionId, interactionToken);
    break;
  }
}
