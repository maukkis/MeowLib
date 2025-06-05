#ifndef _INCLUDE_LOG_H
#define _INCLUDE_LOG_H
#include <string_view>

namespace Log {
  void Log(const std::string_view a);
  constexpr std::string_view fmtrstr = "\e[38;5;212m{:%T} | \e[0;37m {}";
  extern bool enabled;
}

#endif
