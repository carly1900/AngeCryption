#include "../angecryption.h"

#include <stdio.h>
#include <assert.h>
#include <openssl/ssl.h>
#include <stdbool.h>


void test_reverse_iv() {
  char cipher[17] __attribute__((aligned(16))) = "muchsecret123456";
  char plain[17] __attribute__((aligned(16))) =  "suchcrypto123456";
  char key[17] __attribute__((aligned(16))) =    "qualitykey123456";
  char test_cipher[17];
  __m128i iv = reverse_iv(*(__m128i*) plain, *(__m128i*) cipher, *(__m128i*) key);

  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init(&ctx);
  EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, (char*) &iv);
  EVP_CIPHER_CTX_set_padding(&ctx, false);

  int size;
  EVP_EncryptUpdate(&ctx, test_cipher, &size, plain, 16);
  EVP_EncryptFinal_ex(&ctx, test_cipher, &size);
  test_cipher[16] = 0;

  assert(strcmp(test_cipher, cipher) == 0);
}

void test_integrate() {
  long long iv[2];
  assert(angecrypt("test/cat2.jpg", "test/cat.jpg", "qualitykey123456", "test/out.jpg", (char*) &iv) == 0);
  assert(iv[1] == 0x4bbc368e2ad80fb6);
  assert(iv[0] == 0x2d761769191dbb1c);
}

int main(int argc, char* argv[]) {
  test_reverse_iv();
  test_integrate();

  return 0;
}
