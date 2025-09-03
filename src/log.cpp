#include "../include/log.h"
#include <chrono>
#include <cstdio>
#include <print>

namespace Log {
  void dbg(const std::string_view a){
    if(enabled){
       std::println(Log::dbgfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
       std::fflush(stdout);     
    }
  }
  void warn(const std::string_view a){
    std::println(Log::warnfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
    std::fflush(stdout);
  }
  void error(const std::string_view a){
    std::println(stderr, Log::errfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
  }
  void info(const std::string_view a){
    std::println(Log::infofmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
    std::fflush(stdout);
  }
  bool enabled = false;
}
