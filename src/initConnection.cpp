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


#include <cstdint>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/websockets.h>
#include <unistd.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include "includes/eventCodes.h"

void sendIdent(CURL *meow){
  std::cout << "sending ident";
  std::string ident {R"({"op": 2, "d": {"token": "put token here", "intents": 513, "properties": {"os": "linux", "browser": "meowLib", "device": "meowLib"}}})"};
  size_t sent;
  CURLcode res;
  res = curl_ws_send(meow, ident.c_str(), strlen(ident.c_str()), &sent, 0, CURLWS_TEXT);
  std::cout << res << '\n';
  sleep(3);
  char buffer[4096];
  size_t rlen;
  const struct curl_ws_frame *nya;
  curl_ws_recv(meow, buffer, sizeof(buffer), &rlen, &nya);
}


void sendHeartbeat(CURL *meow, std::uint64_t interval){
  float random = ((float) rand()) / (float) RAND_MAX;
  sleep(interval * random / 1000);
  while (true) {
    std::string heartbeat{R"({"op": 1,"d": null})"};
    size_t sent;
    CURLcode res;
    res = curl_ws_send(meow, heartbeat.c_str(), strlen(heartbeat.c_str()), &sent, 0, CURLWS_TEXT);
    if (res == CURLE_OK){
      std::cout << "hearbeat sent succesfully!\n";
    }
    else{
      std::cerr << "something went wrong curlcode is: " << res << '\n';
      return;
    }
    char buffer[4096];
    size_t rlen;
    const struct curl_ws_frame *joo;
    curl_ws_recv(meow, buffer, sizeof(buffer), &rlen, &joo);
    int op;
    try {
      auto meowJson = nlohmann::json::parse(buffer);
      op = meowJson["op"];
    } catch (nlohmann::json::parse_error& e) {} // this is here so it doesnt crash because discord sucks
    if (op == HeartbeatACK || op == Hello){
      std::cout << "server sent ACK\n";
    }
    else {
      std::cerr << "server did not send ACK\n";
    }
    sleep(interval / 1000);
  }
}


std::uint64_t getHeartbeatInterval(CURL *meow){
  char buffer[4096];
  size_t rlen;
  const struct curl_ws_frame *nya;
  // receive data from websocket
  curl_ws_recv(meow, buffer, sizeof(buffer), &rlen, &nya);
  // initialize a json object with the data of buffer
  auto meowJson = nlohmann::json::parse(buffer);
  // create a new json object that has the data of d because discord api sucks
  auto meowNested = meowJson["d"];
  // parse the data of heartbeat_interval into uint64_t and return it
  std::uint64_t interval {meowNested["heartbeat_interval"]};
  return interval;
}

