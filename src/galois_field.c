#include "constants.h"
#include "types.h"

uint8_t g_mul(uint8_t a, uint8_t b) {
  uint8_t p = 0;
  for (; b; b >>= 1){
    if (b & 1)
      p ^= a;

    if (a & 0x80)
      a = (a << 1) ^ GALOIS_PRIME_POLYNOMIAL; 
    else
      a <<= 1;
  }
  return p;
}

uint8_t gf_pow(uint8_t a, const uint8_t exp) {
  uint8_t res = 1;
  for (uint8_t i = 0; i < exp; i++) {
    res = g_mul(res, a);
  }
  return res;
}

// In this program this is only called with a second polynomial of length two where the constant term is 2^i in the galois field and the other term is always 1
// So I am optimizing to assume that is always true since it is
void gf_poly_mul(uint8_t res[MAX_POLYNOMIAL_LEN], uint8_t p[MAX_POLYNOMIAL_LEN], const uint8_t p_len, const uint8_t q1) {
  for (uint8_t i = 0; i < p_len; i++) {
    res[i] ^= p[i];
    res[i+1] ^= g_mul(p[i], q1);
  } 
}

// Takes an unpadded dividend for simplicity, but then res needs to be 0'd so that res can be filled in with only the unpadded dividend
void gf_poly_div(uint8_t res[MAX_POLYNOMIAL_LEN], const uint8_t *dividend, const uint8_t dividend_len, const uint8_t *divisor, const uint8_t divisor_len) {
  for (uint8_t i = 0; i < dividend_len; i++){
    res[i] = dividend[i];
  } 
  for (uint8_t i = 0; i < dividend_len; i++) {
    for (uint8_t j = 1; j < divisor_len; j++) {
        res[i + j] ^= g_mul(divisor[j], res[i]);
    }
  }
}
