#ifndef _INCLUDE_CACHE_H
#define _INCLUDE_CACHE_H
#include "lrucache.h"
#include "log.h"
#include "restclient.h"
#include <chrono>
#include "error.h"
#include <nlohmann/json.hpp>
#include <expected>
#include <string_view>



constexpr auto ttl = std::chrono::minutes(10);
using hrclk = std::chrono::high_resolution_clock;


template<class T>
struct CacheObject {
  CacheObject() = default;
  CacheObject(const T& i){
    object = i;
  }
  T object;
  std::chrono::time_point<hrclk> made = hrclk::now();
};



template<class T> 
class GenericDiscordCache {
public:
  GenericDiscordCache(const std::string_view path, RestClient *ptr)
  : rest{ptr},
    cache(1000),
    path{path}{}
  
  void insert(const std::string& key, const T& item){
    cache.insert(key, item);
  }

  void erase(const std::string& key){
    cache.erase(key);
  }

  std::expected<T, Error> get(const std::string& id, const bool force = false){
    auto item = getFromCache(id);
    if(item && !force) {
      ++hits;
      return *item;
    }
    ++misses;
    Log::dbg("cache miss! fetching from the api");
    auto i = fetchItem(id);
    if(!i) return i;
    cache.insert(id, *i);
    return i;
  }

protected:
  std::expected<T, Error> fetchItem(const std::string& id){
    auto res = rest->get(std::format(APIURL "{}/{}", path, id));
    if(!res.has_value() || res->second != 200){
      auto err = Error(res.value_or(std::make_pair("meowHttp IO error", 0)).first);
      Log::error("failed to get item " + std::string(id));
      err.printErrors();
      return std::unexpected(err);
    }
    return T(nlohmann::json::parse(res->first));
  }

  std::optional<T> getFromCache(const std::string& id){
    auto item = cache.get(id);
    if(!item || hrclk::now() - item->made > ttl) return std::nullopt;
    return item->object;
  }

  RestClient *rest;
  int hits = 0;
  int misses = 0;
  LruCache<std::string, CacheObject<T>> cache;
  std::string path;
};


#endif
