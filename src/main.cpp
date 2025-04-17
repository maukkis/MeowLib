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



#include <csignal>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "includes/nyaBot.h"
#include <iostream>


int main(){
  NyaBot bot;
  SlashCommand slash = SlashCommand("woof", "woofs"); // (commandName, description)
  
  slash.addParam("puppy", "dogSound", STRING, true) // (paramName, valueName, Type)
    .addChoice("woof")
    .addChoice("bark")
    .addChoice("arf")
    .addChoice("wruff");

  bot.addSlash(slash);
  
  bot.onReady([&bot]() {
    bot.syncSlashCommands();
    std::cout << "woofBot is online :3\n";
  });
 

  bot.onSlash([](auto slash){
    if(slash.commandName == "woof"){
      std::string dogSound = slash.parameters["puppy"];
      slash.respond("puppy! " + dogSound);
    }
  });


  bot.run(std::getenv("TOKEN"));
  return 0;
}

