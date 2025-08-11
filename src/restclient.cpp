#include "../include/restclient.h"
#include "../include/nyaBot.h"
#include "../meowHttp/src/includes/https.h"
#include <string>
#include <thread>
#include <utility>


RestClient::RestClient(NyaBot *bot) : bot{bot} {}


std::expected<std::pair<std::string, int>, RestErrors> RestClient::get(const std::string& endpoint)
{
  return sendRawData(endpoint,
              "GET", 
              {"authorization: Bot " + bot->api.token, "content-type: application/json"});
}

std::expected<std::pair<std::string, int>, RestErrors> RestClient::post(const std::string& endpoint,
                                                        const std::string& data)
{
  return sendRawData(endpoint,
              "POST", 
              {"authorization: Bot " + bot->api.token, "content-type: application/json"}, data);
}

std::expected<std::pair<std::string, int>, RestErrors> RestClient::patch(const std::string& endpoint,
                                                        const std::string& data)
{
  return sendRawData(endpoint,
              "PATCH", 
              {"authorization: Bot " + bot->api.token, "content-type: application/json"}, data);
}

std::expected<std::pair<std::string, int>, RestErrors> RestClient::put(const std::string& endpoint,
                                                        const std::string& data)
{
  return sendRawData(endpoint,
              "PUT", 
              {"authorization: Bot " + bot->api.token, "content-type: application/json"}, data);
}

std::expected<std::pair<std::string, int>, RestErrors>
RestClient::sendFormData(const std::string& endpoint,
                         const std::string& data,
                         const std::string& boundary,
                         const std::string& method)
{
  return sendRawData(endpoint,
                     method,
                     {"authorization: Bot " + bot->api.token,
                     "content-type: multipart/form-data; boundary=" + boundary},
                     data);
}


std::expected<std::pair<std::string, int>, RestErrors> 
RestClient::sendRawData(const std::string& endpoint,
                        const std::string& method,
                        const std::vector<std::string>& headers,
                        const std::optional<std::string>& data)
{
  while(true){
    if(rtl.globalLimit){
      Log::Log("we are being globally ratelimited!");
      std::this_thread::sleep_for(std::chrono::seconds(rtl.globalResetAfter.load()));
    }
    if(rtl.userRateLimitTable.contains(endpoint)){
      Log::Log("we are being user ratelimited!");
      auto arf = rtl.userRateLimitTable[endpoint];
      std::this_thread::sleep_for(std::chrono::seconds(arf.resetAfter));
    }

    std::string d;
    auto meow = meowHttp::Https()
      .setUrl(endpoint)
      .setCustomMethod(method)
      .setHeader("user-agent: " + std::string(UserAgent))
      .setWriteData(&d);
    if(data)
      meow.setPostfields(*data);

    for(const auto& a : headers)
      meow.setHeader(a);
    if(meow.perform() != OK){
      return std::unexpected(RestErrors::IOERR);
    }
    if(meow.getLastStatusCode() == 429){
      if(meow.headers["x-ratelimit-global"] == "true"){
        Log::Log("we are being globally ratelimited!");
        rtl.globalLimit.store(true);
        rtl.globalResetAfter.store(std::stoul(meow.headers["retry-after"], nullptr, 10));
        std::this_thread::sleep_for(std::chrono::seconds(rtl.globalResetAfter.load()));
        rtl.globalLimit.store(false);
        rtl.globalResetAfter.store(0);
        continue;
      }
      if(meow.headers["x-ratelimit-scope"] == "user"){
        Log::Log("we are being user ratelimited!");
        rtl.userRateLimitTable[endpoint] = {
          .bucket = meow.headers["x-ratelimit-bucket"],
          .resetAfter = std::stoi(meow.headers["retry-after"], nullptr, 10),
          .reset = std::stoi(meow.headers["x-ratelimit-reset"], nullptr, 10)
        };
        std::this_thread::sleep_for(std::chrono::seconds(rtl.userRateLimitTable[endpoint].resetAfter));
        rtl.userRateLimitTable.erase(endpoint);
        continue;
      }
      else{
        Log::Log("we are being ratelimited on a bucket!");
        std::this_thread::sleep_for(std::chrono::seconds(std::stoi(meow.headers["retry-after"], nullptr, 10)));
        continue;
      }
    }
    
    switch(static_cast<int>(meow.getLastStatusCode())){
      case 403:
        return std::unexpected(RestErrors::Forbidden);
      case 413:
        return std::unexpected(RestErrors::TooLarge);
      case 404:
        return std::unexpected(RestErrors::NotFound);
    }
    return std::make_pair(d, meow.getLastStatusCode());
  }
}
