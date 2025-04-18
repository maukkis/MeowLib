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

  bot.addSlash(SlashCommand("meow", "meows"));

  bot.onReady([&bot]() {
    runOnce(&NyaBot::syncSlashCommands, &bot);
    std::cout << "woofBot is online :3\n";
  });
 

  bot.onSlash([](auto slash){
    if(slash.commandName == "woof"){
      std::string dogSound = slash.parameters["puppy"];
      slash.respond("puppy! " + dogSound);
    }
    if(slash.commandName == "meow"){
      slash.respond("meow");
    }
  });


  bot.run(std::getenv("TOKEN"));
  return 0;
}

