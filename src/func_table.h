#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "syscalls.h"
#include "types.h"
#include "constants.h"

// bits.c
extern uint16_t write_bits(byte *arr, uint16_t curr_bit, uint16_t data, byte bits);

// encode.c
extern uint16_t calculate_total_size_and_get_switches(uint16_t *sizes, byte *data, uint16_t len, struct ModeSegment *segments);
extern void encode_into_codewords(byte *data, struct Version version, byte *codewords, struct ModeSegment *segments, uint16_t segments_len);

// err.c
extern struct ErrData get_err_data(struct Version version);
extern void rs_gen_poly(uint8_t degree, uint8_t *res);
extern void get_full_codewords(struct ErrData err_data, uint8_t* data, uint8_t *res);

// galois_field.c
extern uint8_t gf_add(uint8_t a, uint8_t b);
extern uint8_t gf_mul(uint8_t a, uint8_t b);
extern uint8_t gf_pow(uint8_t a, uint8_t exp);
extern void gf_poly_mul(uint8_t *res, uint8_t *p, uint8_t p_len, uint8_t *q, uint8_t q_len);
extern void gf_poly_div(uint8_t *res, uint8_t *dividend, uint8_t dividend_len, uint8_t *divisor, uint8_t divisor_len);

// main.c
extern int main(int argc, char **argv, char **envp);

// mode.c
extern enum Mode get_mode(byte c);
extern byte ascii_to_alphanumeric(byte c);

// version.c
extern struct Version get_smallest_version(uint16_t *sizes, enum ErrorCorrectionVersion ec);

#endif
