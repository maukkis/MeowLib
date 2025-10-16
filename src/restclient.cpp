#include "../include/restclient.h"
#include "../include/nyaBot.h"
#include <meowHttp/https.h>
#include <string>
#include <thread>
#include <utility>


RestClient::RestClient(NyaBot *bot) : bot{bot} {}


std::expected<std::pair<std::string, int>, RestErrors> RestClient::get(const std::string& endpoint)
{
  return sendRawData(endpoint,
              "GET", 
              {"authorization: Bot " + bot->api.token});
}

std::expected<std::pair<std::string, int>, RestErrors> RestClient::post(const std::string& endpoint,
                                                                        const std::string& data,
                                                                        const std::optional<std::vector<std::string>>& headers)
{
  std::vector<std::string> headerss {
    "authorization: Bot " + bot->api.token,
    "content-type: application/json"
  };

  if(headers)
    headerss.insert(headerss.end(), headers->begin(), headers->end());


  return sendRawData(endpoint, "POST", std::move(headerss), data);
}

std::expected<std::pair<std::string, int>, RestErrors> RestClient::patch(const std::string& endpoint,
                                                                         const std::string& data,
                                                                         const std::optional<std::vector<std::string>>& headers)
{
  std::vector<std::string> headerss {
    "authorization: Bot " + bot->api.token,
    "content-type: application/json"
  };
  
  if(headers)
    headerss.insert(headerss.end(), headers->begin(), headers->end());

  return sendRawData(endpoint, "PATCH", std::move(headerss), data);
}

std::expected<std::pair<std::string, int>, RestErrors> RestClient::put(const std::string& endpoint,
                                                                       const std::optional<std::string>& data,
                                                                       const std::optional<std::vector<std::string>>& headers)
{
  std::vector<std::string> headerss {
    "authorization: Bot " + bot->api.token,
    "content-type: application/json"
  };
  
  if(headers)
    headerss.insert(headerss.end(), headers->begin(), headers->end());

  return sendRawData(endpoint, "PUT", std::move(headerss), data);
}

std::expected<std::pair<std::string, int>, RestErrors> RestClient::deletereq(const std::string& endpoint,
                                                                             const std::optional<std::vector<std::string>>& headers)
{
  std::vector<std::string> headerss {
    "authorization: Bot " + bot->api.token,
  };
  
  if(headers)
    headerss.insert(headerss.end(), headers->begin(), headers->end());

  return sendRawData(endpoint, "DELETE", std::move(headerss));
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
  int retryCount = 0;
  int timeToWait = 5;
  while(!bot->stop){
    if(rtl.globalLimit){
      Log::warn("we are being globally ratelimited!");
      std::this_thread::sleep_for(std::chrono::seconds(rtl.globalResetAfter.load()));
    }
    if(rtl.userRateLimitTable.contains(endpoint)){
      Log::warn("we are being user ratelimited!");
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
      if(retryCount > bot->retryAmount)
        return std::unexpected(RestErrors::IOERR);
      ++retryCount;
      if(timeToWait > 300) timeToWait = 300;
      Log::error("failed to send a http request to endpoint " + endpoint);
      Log::error("waiting for " + std::to_string(timeToWait) + " seconds before retrying");
      std::this_thread::sleep_for(std::chrono::seconds(timeToWait));
      timeToWait *= 2;
      continue;
    }

    if(meow.getLastStatusCode() == 429){
      if(meow.headers["x-ratelimit-global"] == "true"){
        Log::warn("we are being globally ratelimited!");
        rtl.globalLimit.store(true);
        rtl.globalResetAfter.store(std::stoul(meow.headers["retry-after"], nullptr, 10));
        std::this_thread::sleep_for(std::chrono::seconds(rtl.globalResetAfter.load()));
        rtl.globalLimit.store(false);
        rtl.globalResetAfter.store(0);
        continue;
      }

      if(meow.headers["x-ratelimit-scope"] == "user"){
        Log::warn("we are being user ratelimited!");
        std::unique_lock<std::mutex> lock(rtl.rltmtx);
        rtl.userRateLimitTable[endpoint] = {
          .bucket = meow.headers["x-ratelimit-bucket"],
          .resetAfter = std::stoi(meow.headers["retry-after"], nullptr, 10),
          .reset = std::stoi(meow.headers["x-ratelimit-reset"], nullptr, 10)
        };
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(rtl.userRateLimitTable[endpoint].resetAfter));
        lock.lock();
        rtl.userRateLimitTable.erase(endpoint);
        continue;
      }

      else{
        Log::warn("we are being ratelimited on a bucket!");
        std::this_thread::sleep_for(std::chrono::seconds(std::stoi(meow.headers["retry-after"], nullptr, 10)));
        continue;
      }
    }
    
    return std::make_pair(d, meow.getLastStatusCode());
  }
  return std::unexpected(RestErrors::INTERRUPT);
}
