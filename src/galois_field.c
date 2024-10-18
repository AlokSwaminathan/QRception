#include "constants.h"
#include "types.h"

uint8_t g_add(uint8_t a, uint8_t b) {
  return a ^ b;
}

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

uint8_t gf_pow(uint8_t a, uint8_t exp) {
  uint8_t res = 1;
  for (uint8_t i = 0; i < exp; i++){
    res = g_mul(res, a);
  }
  return res;
}

// Res should be a zero'd out array with minimum length p+q-1
void gf_poly_mul(uint8_t *res, uint8_t *p, uint8_t p_len, uint8_t *q, uint8_t q_len) {
  for (uint8_t i = 0; i < p_len; i++){
    for (uint8_t j = 0; j < q_len; j++){
      res[i+j] ^= g_mul(p[i],q[j]);
    }
  } 
}

// Takes an unpadded divident for simplicity, but then res needs to be 0'd
void gf_poly_div(uint8_t *res, uint8_t *dividend, uint8_t dividend_len, uint8_t *divisor, uint8_t divisor_len) {
  for (uint8_t i = 0; i < dividend_len; i++){
    res[i] = dividend[i];
  } 
  for (uint8_t i = 0; i < dividend_len; i++) {
    for (uint8_t j = 1; j < divisor_len; j++) {
        res[i + j] ^= g_mul(divisor[j], res[i]);
    }
  }
}
