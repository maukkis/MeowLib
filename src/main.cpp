#include <csignal>
#include <cstdlib>
#include <memory>
#include <pthread.h>
#include <unistd.h>
#include "includes/commandHandling.h"
#include "includes/nyaBot.h"
#include "includes/slashCommands.h"
#include <iostream>

//define our command handlers
class Woof : public Command {
public:
  void onCommand(SlashCommandInt& i) override {
    const std::string& dogSounds = i.parameters["puppy"];
    i.respond("puppy! " + dogSounds);
  } 
};


class Meow : public Command {
public:
  void onCommand(SlashCommandInt& i) override {
    i.respond("meow");
  } 
};


class Aix : public Command {
public:
  void onCommand(SlashCommandInt& i) override {
    i.respond(AIX);
  } 
private:
   static constexpr char AIX[] {
      "AIX is an UNIX operating system made by IBM for their POWER architecture\nAIX is a cool operating system because\n"
      "1. its fully POSIX compliant\n" 
      "2. it has great management tools like SMIT\n"
      "3. its stable\n"
      "4. its fast and has good support from IBM\ni recommend giving AIX a try!"
    };
};


int main(){
  NyaBot bot;
  SlashCommand slash = SlashCommand("woof", "woofs", USER_INSTALL); // (commandName, description, type)
  
  slash.addParam("puppy", "dogSound", STRING, true) // (paramName, valueName, Type, required)
    .addChoice("woof")
    .addChoice("bark")
    .addChoice("arf")
    .addChoice("wruff");

  bot.addSlash(slash);
  
  bot.addSlash(SlashCommand("meow", "meows", BOTH));

  bot.addSlash(SlashCommand("aix", "what is AIX", BOTH));
  
  bot.onReady([&bot]() {
    runOnce(&NyaBot::syncSlashCommands, &bot);
    std::cout << "woofBot is online :3\n";
  });

  // add command handlers
  bot.addCommandHandler<Woof>("woof"); // you can either pass the class to the template and the command name and optional constructor args
  auto a = std::make_unique<Meow>();
  bot.addCommandHandler("meow", std::move(a)); // or just move an unique ptr to it
  bot.addCommandHandler<Aix>("aix");
/*
  bot.onSlash([](auto slash) {
    if(slash.commandName == "woof"){
      std::string dogSound = slash.parameters["puppy"];
      slash.respond("puppy! " + dogSound);
    }
    if(slash.commandName == "meow"){
      slash.respond("meow");
    }
    if(slash.commandName == "aix"){
      constexpr char AIX[] {
        "AIX is an UNIX operating system made by IBM for their POWER architecture\nAIX is a cool operating system because\n"
        "1. its fully POSIX compliant\n" 
        "2. it has great management tools like SMIT\n"
        "3. its stable\n"
        "4. its fast and has good support from IBM\ni recommend giving AIX a try!"
      };
      slash.respond(AIX);
    }
  });
*/
  bot.run(std::getenv("TOKEN"));
  return 0;
}

