#include <string>
#include <string_view>
#include <format>
#include <vector>
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
