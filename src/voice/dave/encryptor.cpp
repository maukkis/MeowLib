#include "../../../include/voice/dave/encryptor.h"
#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h"
#include <bit>
#include <mutex>
#include <openssl/evp.h>
#include <array>
#include <cstdint>
#include <cstring>

constexpr int truncTagSize = 8;
constexpr uint16_t magicBytes = 0xFAFA;
constexpr int supplementalDataSize = 1;

Encryptor::Encryptor(const std::vector<uint8_t>& key, Dave *dave): dave{dave}, key{key}{}


constexpr uint32_t Encryptor::getMaxHeaderSize(){
  return sizeof(nonce) + truncTagSize + sizeof(magicBytes) + supplementalDataSize;
}

std::vector<uint8_t> Encryptor::uleb128encode(uint64_t a){
  std::vector<uint8_t> buf;
  while(a >= 0x80){
    buf.push_back(0x80 | (a & 0x7F));
    a >>= 7;
  }
  buf.push_back(a);
  return buf;
}



void Encryptor::changeKey(const std::vector<uint8_t>& key){
  std::unique_lock lock(keymtx);
  this->key = key;
}


std::vector<uint8_t> Encryptor::encrypt(const std::vector<uint8_t>& vec){
  std::vector<uint8_t> data(vec.size() + truncTagSize);
  data.reserve(data.size() + getMaxHeaderSize());
  if((nonce >> 24) != previousNonce){
    changeKey(dave->getKeyForGeneration(++generation));
  }
  previousNonce = nonce >> 24;
  int len = aes128GcmEncrypt(vec.data(), vec.size(), data.data());
  if(len == -1){
    Log::error("something went wrong with dave E2EE encryption");
    return {};
  }
  uint32_t nonce = this->nonce++;
  int i{};
  for(const auto& a : uleb128encode(nonce)){
    ++i;
    data.emplace_back(a);
  }
  uint8_t supplementalDataSize = truncTagSize + i + sizeof(magicBytes) + ::supplementalDataSize;
  data.emplace_back(supplementalDataSize);
  for(size_t i = 0; i < sizeof(magicBytes); ++i)
    data.emplace_back(std::bit_cast<uint8_t *>(&magicBytes)[i]);

  return data;
}



int Encryptor::aes128GcmEncrypt(uint8_t const *pt, int ptlen, uint8_t *ct){
  int ctLen = 0;
  int len = 0;
  EVP_CIPHER_CTX *ctx;
  if(!(ctx = EVP_CIPHER_CTX_new())){
    return -1;
  }
  if(EVP_EncryptInit_ex2(ctx, EVP_aes_128_gcm(), nullptr, nullptr, nullptr) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("1 encrypt init fail");
    return -1;
  }
  std::array<uint8_t, 12> aa{0};
  std::memcpy(aa.data() + 8, &nonce, sizeof(nonce));
  if(EVP_EncryptInit_ex2(ctx, nullptr, key.data(), aa.data(), nullptr) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("setting key and iv failed");
    return -1;
  }
  if(EVP_EncryptUpdate(ctx, ct, &len, pt, ptlen) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("encrypt failed");
    return -1;
  }
  ctLen = len;
  if(EVP_EncryptFinal_ex(ctx, ct + len, &len) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("final encrypt failed");
  }
  ctLen += len;

  if(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, truncTagSize, ct + ctLen) != 1){
    Log::error("failed to set tag");
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }
  ctLen += truncTagSize;
  EVP_CIPHER_CTX_free(ctx);
  return ctLen;
}
