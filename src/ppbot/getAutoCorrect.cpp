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


#include <cctype>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>
std::string to_lowercase(const std::string& bark) {
    std::string lower_str{bark};
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return lower_str;
}
std::vector<std::string> getArrayOfOptions(const std::string& value){
  std::ifstream meow{"sigmat.json"};
  std::vector<std::string> songs;
  auto meowJson = nlohmann::json::parse(meow);
  for (const auto& nya: meowJson){
    songs.push_back(nya["Song Name"]);
  }
  std::string valueLower = to_lowercase(value);
  std::vector<std::string> result;
  std::copy_if(songs.begin(), songs.end(), std::back_inserter(result), [&valueLower](const std::string& meow) {
    return to_lowercase(meow).rfind(valueLower, 0) == 0;
  });
  return result;
}

