#include "includes/commandHandling.h"
#include "includes/nyaBot.h"
#include "includes/slashCommandInt.h"
#include "includes/slashCommands.h"
#include <iostream>
#include <string>

//define our command handlers
class Woof : public Command {
public:
  void onCommand(SlashCommandInt& i) override {
    const std::string& dogSounds = i.parameters["puppy"];
    i.respond("puppy! " + dogSounds);
  } 
};

class Say : public Command {
public:
  void onCommand(SlashCommandInt& i) override {
    const std::string& bark = i.parameters["input"];
    if(i.userId == 991406354810015867){
      i.respond(bark);
    } else {
      i.respond("you are not authorized to use this command owo\n*wags tail*", EPHEMERAL);
    }
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
  SlashCommand slash = SlashCommand("woof", "woofs", BOTH); // (commandName, description, type)
   
  slash.addParam("puppy", "dogSound", STRING, true) // (paramName, valueName, Type, required)
    .addChoice("woof")
    .addChoice("bark")
    .addChoice("arf")
    .addChoice("wruff");

  bot.addSlash(slash);
  
  bot.addSlash(SlashCommand("meow", "meows", BOTH));

  bot.addSlash(SlashCommand("aix", "what is AIX", BOTH));

  SlashCommand owo("say", "says something", BOTH);
  owo.addParam("input", "wags tail", STRING, true);
  
  bot.addSlash(owo);

  bot.onReady([&bot]() {
    runOnce(&NyaBot::syncSlashCommands, &bot);
    std::cout << "woofBot is online :3\n";
  });

  // add command handlers
  bot.addCommandHandler<Woof>("woof"); // you can either pass the class to the template and the command name and optional constructor args
  
  auto a = std::make_unique<Meow>();
  bot.addCommandHandler("meow", std::move(a)); // or just move an unique ptr to it

  bot.addCommandHandler<Aix>("aix");
  bot.addCommandHandler<Say>("say");

  // we should still keep this
  bot.onSlash([](auto slash){
    slash.respond("you forgot to add a command handler idiot");
  });

  bot.run(std::getenv("TOKEN"));
  return 0;
}

