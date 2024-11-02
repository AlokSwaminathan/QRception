// Looks like including the .c file directly so gcc can make one .S file is more effective than merging separate ones
// merge_asm still works even though it isnt merging anything anymore
#include <asm/unistd_64.h>

#include "func_table.h"
#include "bits.c"
#include "bmp.c"
#include "constants.c"
#include "constants.h"
#include "encode.c"
#include "err.c"
#include "galois_field.c"
#include "mode.c"
#include "patterns.c"
#include "segments.c"
#include "syscalls.h"
#include "types.h"
#include "version.c"
#include "write_info.c"

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

  uint8_t res_bits[MAX_CODEWORDS * 8];

  // Write white bytes here so if the QR symbol has extra space white will be written
  for (uint16_t i = 0; i < sizeof(res_bits); i++) {
    res_bits[i] = 1;
  }

  get_full_codewords(err, codewords, res_bits);

  uint8_t qr_matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE] = {0};
  
  uint8_t qr_matrix_size = DATA_SIZE(version.version);

  write_patterns(qr_matrix,version.version,qr_matrix_size);

  if (version.version >= MIN_VERSION_INFO_VERSION) {
    write_version_info(qr_matrix, version.version, qr_matrix_size);
  }

  write_format_info(qr_matrix, err_ver, qr_matrix_size);

  write_matrix(qr_matrix, res_bits, version.cw_capacity * 8, qr_matrix_size);


  uint8_t bmp[MAX_BMP_LEN] = {0};

  uint32_t bmp_len = matrix_to_bmp(bmp, qr_matrix, qr_matrix_size);

  syscall3(__NR_write,1,(long) bmp, bmp_len);
}
