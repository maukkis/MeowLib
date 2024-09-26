/* nyaBot simple discord bot written in C++ using libcurl
    Copyright (C) 2024  Luna
     This file is part of nyaBot.

    nyaBot is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    nyaBot is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with nyaBot. If not, see <https://www.gnu.org/licenses/>. */



#ifndef _INIT_CONNECTION_H
#define _INIT_CONNECTION_H
#include <cstdint>
#include <curl/curl.h>
void sendIdent(CURL *meow);
void sendHeartbeat(CURL *meow, std::uint64_t interval);
std::uint64_t getHeartbeatInterval(CURL *meow);
#endif // !DEBUG
