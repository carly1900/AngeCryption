#include "angecryption.h"

#include <openssl/ssl.h>
#include <stdbool.h>


__m128i reverse_iv(__m128i plain, __m128i cipher, __m128i key) {
  char decrypted[16] __attribute__((aligned(16)));

  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init(&ctx);
  EVP_DecryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, (char*) &key, NULL);
  EVP_CIPHER_CTX_set_padding(&ctx, false);

  int size;
  EVP_DecryptUpdate(&ctx, decrypted, &size, (char*) &cipher, 16);
  EVP_DecryptFinal_ex(&ctx, decrypted, &size);

  return _mm_xor_si128(*((__m128i*) decrypted), plain);
}
