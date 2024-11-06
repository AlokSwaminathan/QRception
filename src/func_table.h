#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "syscalls.h"
#include "types.h"
#include "constants.h"

// bits.c
extern uint16_t write_bits(uint8_t *arr, uint16_t curr_bit, uint16_t data, uint8_t bits);

// bmp.c
extern uint32_t matrix_to_bmp(uint8_t *bmp, uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], uint8_t version_size);

// encode.c
extern void encode_into_codewords(uint8_t *data, uint16_t data_len, enum Mode mode, struct Version version, uint8_t *codewords);

// err.c
extern struct ErrData get_err_data(struct Version version);
extern void rs_gen_poly(uint8_t degree, uint8_t *res);
extern void get_full_codewords(struct ErrData err_data, uint8_t* data, uint8_t *res);
extern uint16_t generate_bch_code(uint8_t data);
extern uint32_t generate_golay_code(uint8_t data);

// galois_field.c
extern uint8_t gf_add(uint8_t a, uint8_t b);
extern uint8_t gf_mul(uint8_t a, uint8_t b);
extern uint8_t gf_pow(uint8_t a, uint8_t exp);
extern void gf_poly_mul(uint8_t *res, uint8_t *p, uint8_t p_len, uint8_t *q, uint8_t q_len);
extern void gf_poly_div(uint8_t *res, uint8_t *dividend, uint8_t dividend_len, uint8_t *divisor, uint8_t divisor_len);

// main.c
extern int main(int argc, char **argv, char **envp);

// mode.c
extern enum Mode get_worst_mode(uint8_t* data, uint16_t data_len);
extern uint8_t ascii_to_alphanumeric(uint8_t c);

// segments.c
extern void get_mode_specific_size(uint16_t *sizes, enum Mode mode, uint16_t len);

// symbols.c
extern void write_patterns(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], uint8_t version, uint8_t version_size);

// version.c
extern struct Version get_smallest_version(uint16_t *sizes, enum ErrorCorrectionVersion ec);

// write_info.c
extern void write_matrix(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE],uint8_t *codewords_bits, uint8_t version_size);

#endif
