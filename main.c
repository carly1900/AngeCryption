#include <stdio.h>
#include <wmmintrin.h>
#include <stdbool.h>
#include <string.h>

bool validArguments(int argc, char* argv[], char* errorLog);

int main(int argc, char* argv[]) {
  char* errorLog;
  
  if(!validArguments(argc, argv, errorLog)) {
    printf("Error:%s", errorLog);
    free(errorLog);
    return -1;
  }

  printf("Success!");
  
  return 0;
}

bool validArguments(int argc, char* argv[], char* errorLog) {
  // arguments expected are :
  // [--encrypt:--decrypt] [file input] [optional output file]
  errorLog = "";
  
  // validation of [--encrypt:--decrypt]
  // until proof of contrary
  bool validEncryptionState = false;
  bool hasContradiction = false;
  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "--encrypt") == 0 || strcmp(argv[i], "--decrypt") == 0) {
      if(validEncryptionState) {
        hasContradiction = true;
        break;
      }
      validEncryptionState = true;
    }
  }
  
  if(!validEncryptionState || hasContradiction) {
    char* newError = "\nEncrpytion or decryption mode must be specified using --encrypt or --decrypt.";
    size_t newErrorLength = strlen(newError), errorLogLength = strlen(errorLog);
    size_t errorLogCpyLength;
    char* errorLogCpy = (char*) malloc(errorLogLength + 1);
    memcpy(errorLogCpy, errorLog, errorLogLength + 1);
    errorLogCpyLength = strlen(errorLogCpy);
    
    errorLog = (char*) malloc(newErrorLength + errorLogCpyLength + 1);
    memcpy(errorLog, errorLogCpy, errorLogCpyLength);
    memcpy(errorLog + errorLogCpyLength, newError, newErrorLength + 1);

    free(errorLogCpy);
  }

  // if no error message was printed, the arguments are valid
  return strcmp(errorLog, "") == 0;
}
