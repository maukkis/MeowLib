#include "../include/log.h"
#include <chrono>
#include <cstdio>
#if defined (_AIX)
#include <format>
#include <iostream>
#else
#include <print>
#endif

namespace Log {
  void dbg(const std::string_view a){
    if(enabled){
    #if defined (_AIX)
       std::cout << std::format(Log::dbgfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a) << std::endl;
    #else
       std::println(Log::dbgfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
       std::fflush(stdout);
    #endif
    }
  }
  void warn(const std::string_view a){
  #if defined (_AIX)
    std::cout << std::format(Log::warnfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a) << std::endl;
  #else
    std::println(Log::warnfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
    std::fflush(stdout);
  #endif
  }
  void error(const std::string_view a){
  #if defined (_AIX)
    std::cout << std::format(Log::errfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a) << std::endl;
  #else
    std::println(stderr, Log::errfmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
  #endif
  }
  void info(const std::string_view a){
  #if defined (_AIX)
    std::cout << std::format(Log::infofmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a) << std::endl;
  #else
    std::println(Log::infofmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
    std::fflush(stdout);
  #endif
  }
  bool enabled = false;
}
