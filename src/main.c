// Looks like including the .c file directly so gcc can make one .S file is more effective than merging separate ones
// merge_asm still works even though it isnt merging anything anymore
#include "func_table.h"
#include "galois_field.c"
#include "err.c"
#include "constants.h"
#include "constants.c"
#include "types.h"
#include "bits.c"
#include "encode.c"
#include "mode.c"
#include "patterns.c"
#include "segments.c"
#include "version.c"
#include <asm/unistd_64.h>

int main(int argc, char **argv, char **envp) {
  if (argc < 2 || argc > 3)
    return 1;
  byte *qr_data;
  uint16_t len = 0;
  qr_data = (byte*)argv[1]; 
  if (argc == 2){
    while (*(argv[1]++) != '\0')
      len++;
  } else{
    while (*(argv[2]++) != '\0')
      len = len*10 + *(argv[2]-1) - '0';
  }

  uint16_t sizes[3];
  struct ModeSegment segments[MAX_MODE_SEGMENTS];
  uint16_t segments_len = calculate_total_size_and_get_switches(sizes, qr_data, len, segments);

  enum ErrorCorrectionVersion err_ver = EC_LOW;

  struct Version version = get_smallest_version(sizes,err_ver) ;
  if (version.version == 255) {
    return 1;
  }

  byte codewords[MAX_DATA_CODEWORDS];
  encode_into_codewords(qr_data, version, codewords, segments, segments_len);

  struct ErrData err = get_err_data(version);

  uint8_t res[MAX_CODEWORDS * 8];
  get_full_codewords(err, codewords, res);

  uint8_t qr_matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE] = {0};
  
  uint8_t qr_matrix_size = DATA_SIZE(version.version);

  write_patterns(qr_matrix,version.version,qr_matrix_size);

  syscall3(__NR_write,1,(long) res,version.cw_capacity*8);
  // Get type of data
  // Select smallest qr code version
  // Generate error correction data
  // Set up final message
  // Set up final matrix including the finder patters, separators, and timing patterns
}
