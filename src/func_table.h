#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "syscalls.h"
#include "types.h"

// bits.c
extern uint16_t write_bits(byte *arr, uint16_t curr_bit, uint32_t data, byte bits);

// encode.c
extern uint16_t calculate_total_size_and_get_switches(uint16_t *sizes, byte *data, uint16_t len, struct ModeSegment *segments);
extern uint16_t encode_into_codewords(byte *data, uint16_t data_len, byte *codewords, struct ModeSegment *segments, uint16_t segments_len, enum CharCountVersion cc_version);

// err.c
void get_err_codewords(uint8_t* data, uint8_t data_len, uint8_t* err_codewords, uint8_t* err_codewords_len);
extern struct ErrData get_err_data(struct Version version);

// galois_field.c
extern uint8_t g_add(uint8_t a, uint8_t b);
extern uint8_t g_mul(uint8_t a, uint8_t b);

// main.c
extern int main(int argc, char **argv, char **envp);

// mode.c
extern enum Mode get_mode(byte c);
extern byte ascii_to_alphanumeric(byte c);

// version.c
extern struct Version get_smallest_version(uint16_t *sizes, enum ErrorCorrectionVersion ec);

#endif
