#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <array>
#include <cstdint>
#include <openssl/crypto.h>
#include <openssl/evp.h>

namespace {

static constexpr std::array arff = { 'e', 'x', 'p', 'a', 'n', 'd', ' ', '3',
                                   '2', '-', 'b', 'y', 't', 'e', ' ', 'k' };

constexpr void quarterRound(uint32_t *in, int a, int b, int c, int d){
  in[a] += in[b];
  in[d] = std::rotl(in[d] ^ in[a], 16);
  in[c] += in[d];
  in[b] = std::rotl(in[b] ^ in[c], 12);
  in[a] += in[b];
  in[d] = std::rotl(in[d] ^ in[a], 8);
  in[c] += in[d];
  in[b] = std::rotl(in[b] ^ in[c], 7);
}

void hChaCha20(uint8_t *out, uint8_t *key, uint8_t *nonce){
  std::array<uint32_t, 16> x{0};
  std::memcpy(x.data(), arff.data(), arff.size());
  std::memcpy(&x.at(4), key, 32);
  std::memcpy(&x.at(12), nonce, 16);

  for (size_t i = 0; i < 20; i += 2) {
    quarterRound(x.data(), 0, 4, 8, 12);
    quarterRound(x.data(), 1, 5, 9, 13);
    quarterRound(x.data(), 2, 6, 10, 14);
    quarterRound(x.data(), 3, 7, 11, 15);
    quarterRound(x.data(), 0, 5, 10, 15);
    quarterRound(x.data(), 1, 6, 11, 12);
    quarterRound(x.data(), 2, 7, 8, 13);
    quarterRound(x.data(), 3, 4, 9, 14);
  }
  std::memcpy(out, x.data(), 16);
  std::memcpy(out + 16, &x.at(12), 16);
}


}

int VoiceConnection::aeadxChaCha20Poly1305RtpsizeEncrypt(uint8_t *pt,
                                                         int ptLen,
                                                         uint8_t *key,
                                                         uint8_t *iv,
                                                         uint8_t *aad,
                                                         int aadLen,
                                                         uint8_t *ct)
{
  int ctLen = 0;
  int len = 0;
  EVP_CIPHER_CTX *ctx;
  std::array<uint8_t, 32> derivedKey;
  std::array<uint8_t, 12> newIv{0};
  std::memcpy(newIv.data() + 4, iv + 16, 8);
  hChaCha20(derivedKey.data(), key, iv);
  if(!(ctx = EVP_CIPHER_CTX_new())){
    return -1;
  }
  if(EVP_EncryptInit_ex2(ctx, EVP_chacha20_poly1305(), nullptr, nullptr, nullptr) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("1 encrypt init fail");
    return -1;
  }
  if(EVP_EncryptInit_ex2(ctx, nullptr, derivedKey.data(), newIv.data(), nullptr) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("setting key and iv failed");
    return -1;
  }
  if(EVP_EncryptUpdate(ctx, nullptr, &len, aad, aadLen) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("setting aad failed");
    return -1;
  }
  if(EVP_EncryptUpdate(ctx, ct, &len, pt, ptLen) != 1){
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

  if(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, ct + ctLen) != 1){
    Log::error("failed to set tag");
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }
  ctLen += 16;
  EVP_CIPHER_CTX_free(ctx);
  return ctLen;
}


int VoiceConnection::aeadAes256GcmRtpsizeEncrypt(uint8_t *pt,
                                                 int ptLen,
                                                 uint8_t *key,
                                                 uint8_t *iv,
                                                 [[maybe_unused]]uint8_t *aad,
                                                 [[maybe_unused]]int aadLen,
                                                 uint8_t *ct)
{
  int ctLen = 0;
  int len = 0;
  EVP_CIPHER_CTX *ctx;
  if(!(ctx = EVP_CIPHER_CTX_new())){
    return -1;
  }
  if(EVP_EncryptInit_ex2(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("1 encrypt init fail");
    return -1;
  }
  if(EVP_EncryptInit_ex2(ctx, nullptr, key, iv, nullptr) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("setting key and iv failed");
    return -1;
  }
  if(EVP_EncryptUpdate(ctx, nullptr, &len, aad, aadLen) != 1){
    EVP_CIPHER_CTX_free(ctx);
    Log::error("setting aad failed");
    return -1;
  }
  if(EVP_EncryptUpdate(ctx, ct, &len, pt, ptLen) != 1){
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

  if(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, ct + ctLen) != 1){
    Log::error("failed to set tag");
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }
  ctLen += 16;
  EVP_CIPHER_CTX_free(ctx);
  return ctLen;
}

std::tuple<std::vector<uint8_t>, uint32_t, int> VoiceConnection::transportEncrypt(uint8_t *pt,
                                                                            int ptLen,
                                                                            uint8_t *key,
                                                                            uint8_t *aad,
                                                                            int aadlen)
{
  int len = 0;
  switch(api.cipher){
    case Ciphers::aead_aes256_gcm_rtpsize: {
      std::array<uint8_t, aes256GcmIvSize> nonce{0};
      std::vector<uint8_t> data(tagSize + ptLen);
      uint32_t noncec = api.pNonce++;
      std::memcpy(nonce.data(), &noncec, sizeof(noncec));
      // key = secretKey from discord
      // IV = pNonce padded by 8 null bytes
      // AAD = rtp frame
      // the nonce itself is appended to the end of the payload without any padding
      len = aeadAes256GcmRtpsizeEncrypt(pt,
                                        ptLen,
                                        key,
                                        nonce.data(),
                                        aad,
                                        aadlen,
                                        data.data());
      if(len == -1){
        Log::error("something went wrong with encrypting");
      }
      return {data, noncec, len};
    }
    case Ciphers::aead_xchacha20_poly1305_rtpsize: {
      std::vector<std::uint8_t> data(ptLen + tagSize);
      std::array<uint8_t, xchacha20Poly1305IvSIze> nonce{0};
      uint32_t noncec = api.pNonce++;
      std::memcpy(nonce.data(), &noncec, sizeof(noncec));
      // key = secretKey from discord
      // IV = pNonce padded by 20 null bytes
      // AAD = rtp frame
      // the nonce itself is appended to the end of the payload without any padding
      len = aeadxChaCha20Poly1305RtpsizeEncrypt(pt,
                                                ptLen,
                                                key,
                                                nonce.data(),
                                                aad,
                                                aadlen,
                                                data.data());
      if(len == -1){
        Log::error("something went wrong with encrypting");
      }
      return {data, noncec, len};
    }
  }
  return {};
}
