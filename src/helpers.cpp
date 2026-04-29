#include <chrono>
#include <cstdint>
#include <random>
#include <string>
#include <string_view>
#include <format>
#include <vector>
#if defined(__APPLE__)
#include <ctime>
#endif
#include "../include/helpers.h"
#include "../include/base64.h"


std::string makeFormData(const nlohmann::json j, const std::string_view boundary, const std::vector<Attachment>& a){
  std::string data = std::format("--{}\r\n", boundary);
  data.append("Content-Disposition: form-data; name=\"payload_json\"\r\nContent-Type: application/json\r\n\r\n");
  data.append(j.dump());
  for(size_t i = 0; i < a.size(); ++i){
    data.append(std::format("\r\n--{}\r\n", boundary));
    data.append(std::format("Content-Disposition: form-data; name=\"files[{}]\"; filename=\"{}\"\r\n\r\n", i , a.at(i).name));
    data.append(a.at(i).data);
  }
  data.append(std::format("\r\n--{}--", boundary));
  return data;
}

std::chrono::time_point<std::chrono::system_clock> ISO8601ToTimepoint(const std::string& str){
  #if defined(__APPLE__)
  std::tm t{};
  strptime(str.c_str(), "%FT%T%z", &t);
  size_t start = str.find(".");
  auto time = std::chrono::system_clock::from_time_t(timegm(&t));
  if(start != std::string::npos){
    ++start;
    auto end = str.find("+", start);
    if(end == std::string::npos) return time;
    std::string fractions = str.substr(start, end - start);
    time += std::chrono::microseconds(std::stoi(fractions));
  }
  return time;
  #else
  std::istringstream b{str};
  std::chrono::time_point<std::chrono::system_clock> a;
  // discord usually sends in this format
  b >> std::chrono::parse("%FT%T%Ez", a);
  return a;

  #endif
}

std::string attachmentToDataUri(const Attachment& file){
  std::string type = file.name.substr(file.name.rfind(".")+1);
  std::string dataUri = "data:image/" + lower(type) + ";base64,";
  return dataUri + encodeB64(file.data);
}


std::string& lower(std::string& a){
  std::transform(a.begin(), a.end(), a.begin(), [](char c){
    return std::tolower(c);
  });
  return a;
}

std::string asciiToURLEncoded(const std::string_view a){
  std::string c;
  for(const auto& b : a){
    c += std::format("%{:x}", static_cast<unsigned char>(b));
  }
  return c;
}


std::string generate32ByteASCIINonce(){
  static std::random_device dev;
  static std::mt19937 rnd(dev());
  static std::uniform_int_distribution<> distrib(33, 126);
  std::string a;
  a.reserve(32);
  for(size_t i = 0; i < 32; ++i){
    a += (distrib(rnd));
  }
  return a;
}


int calculateShardId(const std::string& guildId, int numShards){
  uint64_t guildIdInt = std::stoull(guildId, nullptr, 10);
  return (guildIdInt >> 22) % numShards;
}
