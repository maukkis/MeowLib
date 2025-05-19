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
