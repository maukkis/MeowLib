#ifndef _INCLUDE_LOG_H
#define _INCLUDE_LOG_H
#include <string_view>

namespace Log {
  void dbg(const std::string_view a);
  void warn(const std::string_view a);
  void info(const std::string_view a);
  void error(const std::string_view a);
  constexpr std::string_view dbgfmtrstr = "\e[38;5;212mDBG   {:%T} | \e[0;37m {}";
  constexpr std::string_view warnfmtrstr = "\e[0;33mWarn  {:%T} | \e[0;37m {}";
  constexpr std::string_view errfmtrstr = "\e[0;31mError {:%T} | \e[0;37m {}";
  constexpr std::string_view infofmtrstr = "\e[38;5;212mInfo  {:%T} | \e[0;37m {}";
  extern bool enabled;
}

#endif
