#include "constants.h"
#include "func_table.h"
#include "types.h"
#include <asm/unistd_64.h>

long printnum(long num);
int main(int argc, char **argv, char **envp) {
  if (argc < 2)
    return 1;
  byte *qr_data = (byte *)argv[1];
  long len = 0;
  byte *curr = qr_data;
  while (*(curr++) != '\0')
    len++;

  long sizes[3];
  calculate_total_size(sizes, qr_data, len);

  for (long unsigned int i = 0; i < DISTINCT_CHARACTER_COUNT_SIZES; i++) {
    printnum(sizes[i]);
  }
  // Get type of data
  // Select smallest qr code version
  // Generate error correction data
  // Set up final message
  // Set up final matrix including the finder patters, separators, and timing patterns
}

long printnum(long num) {
  char c;
  if (num < 0) {
    c = '-';
    syscall3(__NR_write, 1, (long)&c, 1);
    num *= -1;
  }
  char reverse[100];
  reverse[99] = '\n';
  int count = 0;
  while (num > 0) {
    reverse[sizeof(reverse) - 2 - count] = (num % 10) + 0x30;
    num /= 10;
    count++;
  }
  return syscall3(__NR_write, 1, (long)(reverse + sizeof(reverse) - count - 1), count + 1);
}
