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

void nyaBot::handleSlash(nlohmann::json meowJson){
  CURL *meow = curl_easy_init();
  meowJson = meowJson["d"];
  std::string interactionId = meowJson["id"];
  std::string interactionToken = meowJson["token"];
  auto dataJson = meowJson["data"];
  std::string commandName = dataJson["name"];
  if (commandName == "meow"){
    std::cout << "[*] got slash meow\n";
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    std::string json{R"({"type":4, "data":{"content": "meow"}})"};
    curl_easy_setopt(meow, CURLOPT_HTTPHEADER, headers); 
    curl_easy_setopt(meow, CURLOPT_URL, std::string{"https://discord.com/api/interactions/" + interactionId + '/' + interactionToken + "/callback"}.c_str());
    curl_easy_setopt(meow, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_perform(meow);
    curl_slist_free_all(headers);
  }
  curl_easy_cleanup(meow);
}
