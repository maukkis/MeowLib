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
#include "slashCommandInt.h"
#include "slashCommands.h"
#include <functional>
#include <string>
#include <atomic>
#include <nlohmann/json.hpp>


class NyaBot {
public:
  NyaBot();
  ~NyaBot();

  void run(const std::string& token);
  void addSlash(const SlashCommand& slash);
  void onReady(std::function<void()> f);
  void onSlash(std::function<void(SlashCommandInt)> f);
  void onAutocomplete(std::function<void()> f);
  void syncSlashCommands();
private:
  void sendHeartbeat();
  void listen();
  void connect();
  void sendIdent();
  void getHeartbeatInterval();
  void interaction(nlohmann::json j);
  static void signalHandler(int){
    a->stop.store(true);
  }
  inline static NyaBot *a = nullptr;
  std::function<void(SlashCommandInt)> onSlashF = {};
  std::function<void()> onReadyF = {};
  std::function<void()> onAutocompleteF = {};

  std::atomic<bool> stop{false};
  std::string token;
  meowWs::Websocket handle;


  std::uint64_t interval;
  size_t sequence{0};
  std::string appId;
  std::vector<SlashCommand> slashs;
};
#endif
