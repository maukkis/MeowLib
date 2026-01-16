#ifndef _INCLUDE_VOICE_DAVE_ENCRYPTOR_H
#define _INCLUDE_VOICE_DAVE_ENCRYPTOR_H
#include <mlspp/mls/key_schedule.h>
#include <cstdint>
#include <vector>
#include <string>


class Encryptor {
public:
  Encryptor(const std::vector<uint8_t> derivedKey);
  constexpr uint32_t getMaxHeaderSize();
  std::vector<uint8_t> encrypt(const std::vector<uint8_t>& vec);
private:
  std::vector<uint8_t> key;
  int aes128GcmEncrypt(uint8_t const *pt, int ptlen, uint8_t *ct);
  std::vector<uint8_t> uleb128encode(uint64_t a);
  uint32_t nonce = 0;
};


#endif
