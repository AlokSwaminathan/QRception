#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "syscalls.h"
#include "types.h"

// bits.c
extern uint16_t write_bits(byte *arr, byte offset, uint32_t data, byte bits);

// encode.c
extern uint16_t calculate_total_size_and_get_switches(uint16_t *sizes, byte *data, uint16_t len, struct ModeSegment *segments);
extern uint16_t encode_into_codewords(byte *data, uint16_t data_len, byte *codewords, struct ModeSegment *segments, uint16_t segments_len, byte cc_version);

// main.c
extern int main(int argc, char **argv, char **envp);

// mode.c
extern enum Mode get_mode(byte c);
byte ascii_to_alphanumeric(byte c);
#endif
