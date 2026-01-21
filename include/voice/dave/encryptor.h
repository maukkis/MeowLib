#ifndef _INCLUDE_VOICE_DAVE_ENCRYPTOR_H
#define _INCLUDE_VOICE_DAVE_ENCRYPTOR_H
#include <atomic>
#include <mlspp/mls/key_schedule.h>
#include <cstdint>
#include <mutex>
#include <vector>
class Dave;

class Encryptor {
public:
  Encryptor(const std::vector<uint8_t>& derivedKey, Dave *dave);
  constexpr uint32_t getMaxHeaderSize();
  std::vector<uint8_t> encrypt(const std::vector<uint8_t>& vec);
private:
  Dave *dave;
  void changeKey(const std::vector<uint8_t>& key);
  std::mutex keymtx;
  std::vector<uint8_t> key;
  int aes128GcmEncrypt(uint8_t const *pt, int ptlen, uint8_t *ct);
  std::vector<uint8_t> uleb128encode(uint64_t a);
  std::atomic<uint32_t> nonce = 0;
  std::atomic<uint8_t> previousNonce = 0;
  std::atomic<int> generation = 0;
  friend Dave;
};


#endif
