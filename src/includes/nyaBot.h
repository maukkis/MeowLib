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
#include "../../meowHttp/src/includes/websocket.h"
#include <csignal>
#include <cstdint>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include <iostream>
class nyaBot {
public:
  nyaBot(std::string tokenya);
  ~nyaBot();
private:
  void sendHeartbeat();
  void listen();
  void handleSlash(nlohmann::json meowJson);
  void connect();
  void sendIdent();
  void getHeartbeatInterval();
  bool stop{false};
  const std::string token;
  meowWs::Websocket handle;
  std::uint64_t interval;
  size_t sequence{0};
  std::string appId;
};
#endif
