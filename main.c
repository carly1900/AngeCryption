#include <stdio.h>
#include <wmmintrin.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

//utilities
char* concat(char* first, char* second);

//subdivision of the program
bool validArguments(int argc, char* argv[], char** errorLog);

//aes functions
__m128i keyExpansion(__m128i precedingRoundKey, __m128i referenceKey);


int main(int argc, char* argv[]) {
  char* errorLog = "";
  
  if(!validArguments(argc, argv, &errorLog)) {
    printf("Error:%s", errorLog);
    free(errorLog);
    return -1;
  }

  // keygen
  // set the 128 bits key as two aligned in memory 64 bits varialbes
  uint64_t rawKey[2] __attribute__((aligned(16))) = {0x8930295884727458, 0x2932771649534568};
  __m128i key = _mm_load_si128((__m128i*) rawKey);
   
  __m128i key1 = keyExpansion(key, _mm_aeskeygenassist_si128(key, 0x01));
  __m128i key2 = keyExpansion(key1, _mm_aeskeygenassist_si128(key1, 0x02));
  __m128i key3 = keyExpansion(key2, _mm_aeskeygenassist_si128(key2, 0x04));
  __m128i key4 = keyExpansion(key3, _mm_aeskeygenassist_si128(key3, 0x08));
  __m128i key5 = keyExpansion(key4, _mm_aeskeygenassist_si128(key4, 0x10));
  __m128i key6 = keyExpansion(key5, _mm_aeskeygenassist_si128(key5, 0x20));
  __m128i key7 = keyExpansion(key6, _mm_aeskeygenassist_si128(key6, 0x40));
  __m128i key8 = keyExpansion(key7, _mm_aeskeygenassist_si128(key7, 0x80));
  __m128i key9 = keyExpansion(key8, _mm_aeskeygenassist_si128(key8, 0x1B));
  __m128i key10 = keyExpansion(key9, _mm_aeskeygenassist_si128(key9, 0x36));

  // encryption
  uint16_t rawMsg __attribute__((aligned(16))) = 0x5234;
  printf("\nplaintext %d", rawMsg);
  
  __m128i msg = _mm_load_si128((__m128i*) &rawMsg);
  msg = _mm_xor_si128(msg, key);
  msg = _mm_aesenc_si128(msg, key1);
  msg = _mm_aesenc_si128(msg, key2);
  msg = _mm_aesenc_si128(msg, key3);
  msg = _mm_aesenc_si128(msg, key4);
  msg = _mm_aesenc_si128(msg, key5);
  msg = _mm_aesenc_si128(msg, key6);
  msg = _mm_aesenc_si128(msg, key7);
  msg = _mm_aesenc_si128(msg, key8);
  msg = _mm_aesenc_si128(msg, key9);
  msg = _mm_aesenclast_si128(msg, key10);

  uint16_t rawCipher __attribute__((aligned(16)));
  _mm_store_si128((__m128i*) &rawCipher, msg);
  
  printf("\nCypher: %d", rawCipher);
  
  return 0;
}

bool validArguments(int argc, char* argv[], char** errorLog) {
  // expected arguments are :
  // [--encrypt:--decrypt] [file input] [optional output file]
  
  // validation of [--encrypt:--decrypt]
  // until proof of contrary
  bool validEncryptionState = false;
  bool hasContradiction = false;
  for(int i = 1; i < argc; ++i) {
    if(strcmp(argv[i], "--encrypt") == 0 || strcmp(argv[i], "--decrypt") == 0) {
      if(validEncryptionState) {
        hasContradiction = true;
        break;
      }
      validEncryptionState = true;
    }
  }
  
  if(hasContradiction) {
    *errorLog = concat(*errorLog, "\nEcryption state cannot be resolved, use --encrypt or (exclusive) --decrypt once.");
  } else if(!validEncryptionState) {
    *errorLog = concat(*errorLog, "\nEncrpytion or decryption mode must be specified using --encrypt or (exclusive) --decrypt.");
  }

  // validation of [file input]
  // iteration over the argument vector could be done only once
  // for efficency, but readability was preferred
  int possibleFileNameCpt = 0;
  for(int i = 1; i < argc; ++i) {
    if(argv[i][0] != '-') {
      // TODO regex? identifier -i -o?
      ++possibleFileNameCpt;
    }
  }

  if(possibleFileNameCpt == 0) {
    *errorLog = concat(*errorLog, "\nNo output file name was detected arguments must be: [--encrypt:--decrypt] [file input] [optional output file].");
  } else if(possibleFileNameCpt > 2) {
    *errorLog = concat(*errorLog, "\nInput and output file names cannot be resolved.\nArguments must be: [--encrypt:--decrypt] [file input] [optional output file].");
  }
  
  // if no error message was printed, the arguments are valid
  return strlen(*errorLog) == 0;
}

char* concat(char* first, char* second) {
  size_t firstLength = strlen(first), secondLength = strlen(second);
  char* concatenated = (char*) malloc(firstLength + secondLength + 1);

  memcpy(concatenated, first, firstLength);
  memcpy(concatenated + firstLength, second, secondLength + 1);
  
  return concatenated;
}

__m128i keyExpansion(__m128i precedingRoundKey, __m128i referenceKey) {
  precedingRoundKey = _mm_xor_si128(precedingRoundKey, _mm_slli_si128(precedingRoundKey, 4));
  precedingRoundKey = _mm_xor_si128(precedingRoundKey, _mm_slli_si128(precedingRoundKey, 4));
  precedingRoundKey = _mm_xor_si128(precedingRoundKey, _mm_slli_si128(precedingRoundKey, 4));
  referenceKey = _mm_shuffle_epi32(referenceKey, _MM_SHUFFLE(3,3,3,3));
  return _mm_xor_si128(precedingRoundKey, referenceKey);
}
