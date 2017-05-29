#include "angecryption.h"

#include <openssl/ssl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


__m128i reverse_iv(__m128i plain, __m128i cipher, __m128i key) {
  char decrypted[16] __attribute__((aligned(16)));

  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init(&ctx);
  EVP_DecryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, (char*) &key, NULL);
  EVP_CIPHER_CTX_set_padding(&ctx, false);

  int size;
  EVP_DecryptUpdate(&ctx, decrypted, &size, (char*) &cipher, 16);
  EVP_DecryptFinal(&ctx, decrypted, &size);

  return _mm_xor_si128(*(__m128i*) decrypted, plain);
}

int angecrypt(const char* encrypt_file, const char* fake_file, const char* key, const char* result_file, char* const iv) {
  struct stat st_enc;
  struct stat st_fake;
  if(stat(encrypt_file, &st_enc) != 0 || stat(fake_file, &st_fake) != 0) {
    return -1;
  }

  // encrypted size will be a multiple of 16 bytes
  int enc_size = st_enc.st_size + (16 - st_enc.st_size%16);

  /* reverse iv from first chunk */
  int encrypt_fd = open(encrypt_file, O_RDONLY);
  int result_fd = open(result_file, O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
  char aligned_key[16] __attribute__((aligned(16)));
  char chunk[16] __attribute__((aligned(16)));
  short cipher[8] __attribute__((aligned(16)));
  cipher[0] = 0xd8ff; // jpeg magic bytes
  cipher[1] = 0xfeff; // comment chunk id
  int commentchunksize = enc_size - 6;
  cipher[2] = ((commentchunksize >> 8) | (commentchunksize << 8)) & 0xffff;
  memcpy(aligned_key, key, 16);
  int readsize = read(encrypt_fd, chunk, 16);
  __m128i xmm_iv = reverse_iv(*(__m128i*) chunk, *(__m128i*) cipher, *(__m128i*) aligned_key);

  *(__m128i* const) iv = xmm_iv;

  /* encrypt to file */
  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init(&ctx);
  EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, (char*) key, iv);
  int encryptedsize;
  char buffer[16];
  do {
    EVP_EncryptUpdate(&ctx, buffer, &encryptedsize, chunk, readsize);
    write(result_fd, buffer, encryptedsize);
    readsize = read(encrypt_fd, chunk, 16);
  } while(readsize != -1 && readsize != 0);
  EVP_EncryptFinal(&ctx, buffer, &encryptedsize);
  write(result_fd, buffer, encryptedsize);
  close(encrypt_fd);

  /* append fake jpeg */
  int fake_fd = open(fake_file, O_RDONLY);
  // consume jpeg magic bytes
  readsize = read(fake_fd, buffer, 2);
  while(readsize != -1 && readsize != 0) {
    readsize = read(fake_fd, buffer, 16);
    write(result_fd, buffer, readsize);
  }
  close(fake_fd);
  close(result_fd);

  return 0;
}
