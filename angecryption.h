#include <wmmintrin.h>


__m128i reverse_iv(__m128i plain, __m128i cipher, __m128i key);
int angecrypt(const char* encrypt_file, const char* fake_file, const char* key, const char* result_file, char* const iv);
