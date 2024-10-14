#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "syscalls.h"
#include "types.h"

extern int main(int argc, char **argv, char **envp);

extern enum Mode get_mode(unsigned char c);
extern long calculate_total_size_and_get_switches(long *sizes, byte *data, long len, struct ModeSegment *segments);

void write_bits(byte *arr, byte offset, unsigned int data, byte bits);

#endif
