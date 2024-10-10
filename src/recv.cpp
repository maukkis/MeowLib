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



#include <cstring>
#include <fstream>
#include <iostream>
#include "includes/nyaBot.h"
#include <nlohmann/json.hpp>
#include "includes/eventCodes.h"
#include <unistd.h>
#include <stdio.h>


void nyaBot::listen(){
  std::ofstream meowlog{"meow.log"};
  while (true){
    char buffer[8192];
    size_t rlen;
    const struct curl_ws_frame *nya;
    curl_ws_recv(meow, buffer, sizeof(buffer)-1, &rlen, &nya);
    if (rlen < 1){
      continue;
    }
    else {
      std::cout << "[*] received: " << rlen << " bytes\n";
      parseAgain:
      buffer[rlen] = '\0';
      try {
        auto meowJson = nlohmann::json::parse(buffer);
        size_t op = meowJson["op"];
        switch(op){
          case HeartbeatACK:
            std::cout << "[*] server sent ACK\n";
          break;
          case Dispatch:
            std::string meow = meowJson["t"];
            if (meow == "READY"){
              sequence = meowJson["s"];
              std::cout << "[*] got ready!\n";
            }
            else if(meow == "GUILD_CREATE"){
              sequence = meowJson["s"];
              std::cout << "[*] got guild info!\n";
            }
            else {
              std::cout << "[!] got unknown request printing to log\n";
              meowlog << buffer << '\n';
              sequence = meowJson["s"];
            }
          break;
        }
      }
      catch(nlohmann::json::exception& e){
        std::cout << "[!] got parse error :( partial data?\n";
        std::cout << "[*] trying to receive again\n";
        char buffer1[8192];
        size_t rlen1;
        const struct curl_ws_frame *nya1;
        curl_ws_recv(meow, buffer1, sizeof(buffer1)-1, &rlen1, &nya1);
        if (rlen1 < 1){
          std::cout << "[!] buffer printed to log\n";
          meowlog << buffer << std::endl;
          continue;
        }
        else {
          std::cout << "[*] got data: " << rlen1 << " bytes\n";
          buffer[rlen] = static_cast<char>(NULL);
          rlen += rlen1;
          strncat(buffer, buffer1, rlen1);
          goto parseAgain;
        }
 
      }
    }
  }
}
