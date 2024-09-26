/* nyaBot simple discord bot written in C++ using libcurl
    Copyright (C) 2024  Luna
This file is part of nyaBot.

nyaBot is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

nyaBot is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with nyaBot; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */



#ifndef _EVENT_CODES_H
#define _EVENT_CODES_H
enum eventCodes{
  Dispatch = 0,
  Heartbeat = 1,
  Identify = 2,
  PresenceUpdate = 3,
  VoiceStateUpdate = 4,
  Resume = 6,
  Reconnect = 7,
  RequestGuildMember = 8,
  InvalidSession = 9,
  Hello = 10,
  HeartbeatACK = 11
};


#endif
