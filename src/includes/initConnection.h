#ifndef _INIT_CONNECTION_H
#define _INIT_CONNECTION_H
#include <cstdint>
#include <curl/curl.h>
void sendIdent(CURL *meow);
void sendHeartbeat(CURL *meow, std::uint64_t interval);
std::uint64_t getHeartbeatInterval(CURL *meow);
#endif // !DEBUG
