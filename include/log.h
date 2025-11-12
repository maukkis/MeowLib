#ifndef _INCLUDE_LOG_H
#define _INCLUDE_LOG_H
#include <string_view>

namespace Log {
  void dbg(const std::string_view a);
  void warn(const std::string_view a);
  void info(const std::string_view a);
  void error(const std::string_view a);
  constexpr std::string_view dbgfmtrstr = "\x1b[38;5;212mDBG   {:%T} | \x1b[0;37m {}";
  constexpr std::string_view warnfmtrstr = "\x1b[0;33mWarn  {:%T} | \x1b[0;37m {}";
  constexpr std::string_view errfmtrstr = "\x1b[0;31mError {:%T} | \x1b[0;37m {}";
  constexpr std::string_view infofmtrstr = "\x1b[38;5;212mInfo  {:%T} | \x1b[0;37m {}";
  extern bool enabled;
}

#endif
