#include "../../include/voice/voiceconnection.h"
#include "../../include/log.h"
#include <openssl/crypto.h>
#include <openssl/evp.h>


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
    Log::error("1 encrypt init fail");
    return -1;
  }
  if(EVP_EncryptInit_ex2(ctx, nullptr, key, iv, nullptr) != 1){
    Log::error("setting key and iv failed");
    return -1;
  }
  if(EVP_EncryptUpdate(ctx, nullptr, &len, aad, aadLen) != 1){
    Log::error("setting aad failed");
    return -1;
  }
  if(EVP_EncryptUpdate(ctx, ct, &len, pt, ptLen) != 1){
    Log::error("encrypt failed");
    return -1;
  }
  ctLen = len;
  if(EVP_EncryptFinal_ex(ctx, ct + len, &len) != 1){
    Log::error("final encrypt failed");
  }
  ctLen += len;

  if(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, ct + ctLen) != 1){
    Log::error("failed to set tag");
    return -1;
  }
  ctLen += 16;
  EVP_CIPHER_CTX_free(ctx);
  return ctLen;
}
