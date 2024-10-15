// Looks like including the .c file directly so gcc can make one .S file is more effective than merging separate ones
// merge_asm still works even though it isnt merging anything anymore
#include "constants.h"
#include "constants.c"
#include "func_table.h"
#include "types.h"
#include "bits.c"
#include "encode.c"
#include "mode.c"
#include "version.c"
#include <asm/unistd_64.h>

long printnum(long num);
int main(int argc, char **argv, char **envp) {
  if (argc < 2)
    return 1;
  byte *qr_data = (byte *)argv[1];
  byte *curr = qr_data;
  uint32_t len = 0;
  while (*(curr++) != '\0')
    len++;
  
  if (len > UINT16_MAX){
    return 1;
  }

  uint16_t sizes[3];
  struct ModeSegment segments[MAX_MODE_SEGMENTS];
  uint16_t segments_len = calculate_total_size_and_get_switches(sizes, qr_data, len, segments);

  enum ErrorCorrectionVersion err = EC_LOW;

  struct Version version = get_smallest_version(sizes,err) ;
  if (version.version == 255) {
    return 1;
  }

  byte codewords[MAX_CODEWORDS];
  len = encode_into_codewords(qr_data, len, codewords, segments, segments_len, version.cc_version);

  syscall3(__NR_write,1,(long) codewords,len);
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
