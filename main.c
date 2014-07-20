#include <stdio.h>
#include <wmmintrin.h>
#include <stdbool.h>
#include <string.h>

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
  
  printf("Success!");
  
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
