#include <cstdint>
#ifndef _INCLUDE_VOICE_VOICEOPCODES_H
#define _INCLUDE_VOICE_VOICEOPCODES_H
enum VoiceOpcodes : uint8_t {
  IDENTIFY,
  SELECT_PROTOCOL,
  READY,
  HEARTBEAT,
  SESSION_DESCRIPTION,
  SPEAKING,
  HEARTBEAT_ACK,
  CLIENT_CONNECT = 11,
  DAVE_Prepare_Transition = 21,
  DAVE_Execute_Transition = 22,
  DAVE_Transition_Ready = 23,
  DAVE_Prepare_Epoch = 24,
  DAVE_MLS_External_Sender = 25,
  DAVE_MLS_Key_Package = 26,
  DAVE_MLS_Proposals = 27,
  DAVE_MLS_Commit_Welcome = 28,
  DAVE_MLS_Announce_Commit_Transition = 29,
  DAVE_MLS_Welcome = 30,
  DAVE_MLS_Invalid_Commit_Welcome = 31,
};
#endif
