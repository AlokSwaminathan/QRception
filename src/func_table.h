#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "syscalls.h"
#include "types.h"
#include "constants.h"

// bits.c
uint16_t write_bits(uint8_t *arr, const uint16_t curr_bit, const uint16_t data, const uint8_t bits);

// bmp.c
uint32_t matrix_to_bmp(uint8_t *bmp, const uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t version_size);

// encode.c
void encode_into_codewords(const uint8_t data[MAX_DATA_CODEWORDS], const uint16_t data_len, const enum Mode mode, const struct Version version, uint8_t data_codewords[MAX_DATA_CODEWORDS]);

// err.c
struct ErrData get_err_data(const struct Version version);
void get_full_codewords(const struct ErrData err_data, const uint8_t data[MAX_DATA_CODEWORDS], uint8_t res[MAX_CODEWORDS_BITS]);
uint16_t generate_bch_code(const uint8_t data);
uint32_t generate_golay_code(const uint8_t data);

// galois_field.c
uint8_t gf_mul(uint8_t a, uint8_t b);
uint8_t gf_pow(uint8_t a, const uint8_t exp);
void gf_poly_mul(uint8_t res[MAX_POLYNOMIAL_LEN], uint8_t p[MAX_POLYNOMIAL_LEN], const uint8_t p_len, const uint8_t q1);
void gf_poly_div(uint8_t res[MAX_POLYNOMIAL_LEN], const uint8_t *dividend, const uint8_t dividend_len, const uint8_t *divisor, const uint8_t divisor_len);

// main.c
int main(int argc, char **argv, char **envp);

// mode.c
enum Mode get_worst_mode(const uint8_t data[MAX_DATA_CODEWORDS], const uint16_t data_len);
uint8_t ascii_to_alphanumeric(uint8_t c);

// segments.c
void get_mode_specific_size(uint16_t sizes[3], const enum Mode mode, const uint16_t len);

// symbols.c
void write_patterns(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t version, const uint8_t version_size);

// version.c
struct Version get_smallest_version(const uint16_t sizes[3], const enum ErrorCorrectionVersion ec);

// write_info.c
void write_matrix(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t codewords_bits[MAX_CODEWORDS_BITS], const uint8_t version_size);
void write_version_info(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t version, const uint8_t version_size);
void write_format_info(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const enum ErrorCorrectionVersion ec, const uint8_t version_size);

#endif
