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

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

double calculate(double G2) {
    if (G2 > 900000) {
        return 0.91 + (0.135 * ((G2 - 900000) / 100000.0));
    } else if (G2 > 800000) {
        return 0.75 + (0.16 * ((G2 - 800000) / 100000.0));
    } else if (G2 > 700000) {
        return 0.55 + (0.2 * ((G2 - 700000) / 100000.0));
    } else {
        return 0.275 * std::max(0.0, (G2 - 500000) / 100000.0);
    }
}
double calculateAccBonus(double strainBase, double normalizedscore){
  double accBonus;
  if (normalizedscore > 970000){
    return 0.1 * strainBase * std::pow(((normalizedscore - 970000) / 30000.0), 1.1);
  }
  return 0;
}

double calculateStrain(double strainBase, double multiplier){
  char homo[40];
  sprintf(homo, "%.2f", strainBase * multiplier);
  return atof(homo);
}

double calculatePP(double strain, double accBonus){
  return std::pow(std::pow(strain, 1.12) + std::pow(accBonus, 1.11), 1/1.08) * 1.1;
}

double calculateNormalizedScore(double score, double maxScore){
  return (score * 1000000) / maxScore;
}

double getPP(const std::string& songName, const std::string& instrument, const double& score){
  std::ifstream meow{"sigmat.json"};
  auto meoww = nlohmann::json::parse(meow);
  double pp = 0;
  for (const auto& nya : meoww) {
    if(songName.c_str() == nya["Song Name"]){
      std::cout << "got name\n";
      if (strcmp(instrument.c_str(), "drums") == 0){
        double maxScore = nya["Drums (40%)"];
        double normalizedscore = calculateNormalizedScore(score, maxScore);
        double strainMultiplier = calculate(normalizedscore);
        double strainBase = nya["__11"];
        double accBonus = calculateAccBonus(strainBase, normalizedscore);
        double strain = calculateStrain(strainBase, strainMultiplier);
        pp = calculatePP(strain, accBonus);
      }
      else if(strcmp(instrument.c_str(), "vocals") == 0){
        double maxScore = nya["Vocals (40%)"];
        double normalizedscore = calculateNormalizedScore(score, maxScore);
        double strainMultiplier = calculate(normalizedscore);
        double strainBase = nya["__2"];
        double accBonus = calculateAccBonus(strainBase, normalizedscore);
        double strain = calculateStrain(strainBase, strainMultiplier);
        pp = calculatePP(strain, accBonus);
      } 
      else if(strcmp(instrument.c_str(), "lead") == 0){
        double maxScore = nya["Lead (40%)"];
        double normalizedscore = calculateNormalizedScore(score, maxScore);
        double strainMultiplier = calculate(normalizedscore);
        double strainBase = nya["__8"];
        double accBonus = calculateAccBonus(strainBase, normalizedscore);
        double strain = calculateStrain(strainBase, strainMultiplier);
        pp = calculatePP(strain, accBonus);

      }
      else if(strcmp(instrument.c_str(), "bass") == 0){
        double maxScore = nya["Bass (40%)"];
        double normalizedscore = calculateNormalizedScore(score, maxScore);
        double strainMultiplier = calculate(normalizedscore);
        double strainBase = nya["__5"];
        double accBonus = calculateAccBonus(strainBase, normalizedscore);
        double strain = calculateStrain(strainBase, strainMultiplier);
        pp = calculatePP(strain, accBonus);

      }
      else {
        std::cout << "bona syö sut\n";
      }
    }
  }
  return pp;
}
