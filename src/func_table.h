#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "syscalls.h"
#include "types.h"

extern int main(int argc, char **argv, char **envp);

extern enum Mode get_mode(byte c);
extern long calculate_total_size_and_get_switches(uint16_t *sizes, byte *data, uint16_t len, struct ModeSegment *segments);

void write_bits(byte *arr, byte offset, uint32_t data, byte bits);

#endif
