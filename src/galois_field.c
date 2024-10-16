#include "constants.h"
#include "types.h"

uint8_t g_add(uint8_t a, uint8_t b) {
  return a ^ b;
}

uint8_t g_mul(uint8_t a, uint8_t b) {
  uint8_t p = 0;
  while (a != 0 && b != 0){
    if (b & 1)
      p ^= a;

    if (a & 0x80)
      a = (a << 1) ^ GALOIS_PRIME_POLYNOMIAL; 
    else
      a <<= 1;
    b >>= 1;
  }
  return p;
}
