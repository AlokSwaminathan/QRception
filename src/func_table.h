#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "syscalls.h"
#include "types.h"

extern int main(int argc, char **argv, char **envp);

extern enum Mode get_mode(unsigned char c);
extern void calculate_total_size(long *sizes, byte *msg, long len);
extern long calculate_total_size_and_get_switches(long *sizes, byte *msg, long len, struct ModeSegment *segments);

#endif
