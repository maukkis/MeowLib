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




#ifndef nyaBot_H
#define nyaBot_H

#include <cstdint>
#include <string>
#include <curl/curl.h>
#include <thread>
#include <iostream>
class nyaBot {
public:
  nyaBot(std::string tokenya) : token{tokenya}{
    meow = curl_easy_init();
    curl_easy_setopt(meow, CURLOPT_CONNECT_ONLY, 2L);
    curl_easy_setopt(meow, CURLOPT_URL, "wss://gateway.discord.gg");
    connect();
    getHeartbeatInterval();
    std::cout << interval;
    sendIdent();
  }
  ~nyaBot(){
    // close the websocket
    size_t sent;
    (void)curl_ws_send(meow, "", 0, &sent, 0, CURLWS_CLOSE);
  }
  void sendHeartbeat();

private:
  void connect();
  void sendIdent();
  void getHeartbeatInterval();
  CURL *meow;
  std::string token;
  std::uint64_t interval;
};
#endif
