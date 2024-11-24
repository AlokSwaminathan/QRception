#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "constants.h"
#include "syscalls.h"
#include "types.h"

// bits.c
uint16_t write_bits(uint8_t *arr, const uint16_t curr_bit, const uint16_t data, const uint8_t bits);

// bmp.c
uint32_t matrix_to_bmp(uint8_t *bmp, const uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE]);

// encode.c
void encode_into_codewords(const uint8_t data[MAX_DATA_CODEWORDS], const uint16_t data_len, const enum Mode mode, uint8_t data_codewords[MAX_DATA_CODEWORDS]);

// err.c
void get_full_codewords(const uint8_t data[MAX_DATA_CODEWORDS], uint8_t res[MAX_CODEWORDS_BITS]);
uint16_t generate_bch_code(const uint8_t data);
uint32_t generate_golay_code(const uint8_t data);

// galois_field.c
uint8_t gf_mul(uint8_t a, uint8_t b);
void gf_poly_div(uint8_t res[MAX_POLYNOMIAL_LEN], const uint8_t *dividend, const uint8_t dividend_len, const uint8_t *divisor, const uint8_t divisor_len);

// main.c
int main(int argc, char **argv, char **envp);

// mode.c
enum Mode get_worst_mode(const uint8_t data[MAX_DATA_CODEWORDS], const uint16_t data_len);
uint8_t ascii_to_alphanumeric(uint8_t c);

// symbols.c
void write_patterns(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE]);

// write_info.c
void write_matrix(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t codewords_bits[MAX_CODEWORDS_BITS]);
void write_version_info(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE]);
void write_format_info(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE]);

#endif
