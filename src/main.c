// Looks like including the .c file directly so gcc can make one .S file is more effective than merging separate ones
// merge_asm still works even though it isnt merging anything anymore
#include <asm/unistd_64.h>
#include <fcntl.h>

#include "bits.c"
#include "bmp.c"
#include "constants.c"
#include "constants.h"
#include "encode.c"
#include "err.c"
#include "func_table.h"
#include "galois_field.c"
#include "mode.c"
#include "patterns.c"
#include "segments.c"
#include "syscalls.h"
#include "types.h"
#include "version.c"
#include "write_info.c"

int main(int argc, char **argv) {
  uint8_t qr_data[MAX_DATA_CODEWORDS];
  uint16_t len = 0;

  // This is when the input is passed as a string
  if (argc == 2) {
    uint8_t *qr_curr = (uint8_t *)qr_data;
    while (*argv[1] != '\0') {
      *(qr_curr++) = *(argv[1]++);
    }
    len = qr_curr - (uint8_t *)qr_data;
  }
  // This is if the input is a file path
  else if (argc == 3) {
    long fd = syscall3(__NR_open, (long)argv[2], O_RDONLY, 0);
    // No error checking, we don't have space for that
    len = syscall3(__NR_read, fd, (long)qr_data, sizeof(qr_data));
    syscall1(__NR_close, fd);
  }

  // 3 different version splits where the mode size lengths are different, so keeping those in mind is important to find the final size of the qr code bitstream
  uint16_t sizes[DISTINCT_CHARACTER_COUNT_SIZES];
  const enum Mode mode = get_worst_mode(qr_data, len);

  get_mode_specific_size(sizes, mode, len);

  const enum ErrorCorrectionVersion err_ver = EC_LOW;

  const struct Version version = get_smallest_version(sizes, err_ver);

  // 255 is the return value if there is no valid version found
  if (version.version == 255) {
    return 1;
  }

  uint8_t codewords[MAX_DATA_CODEWORDS];
  encode_into_codewords(qr_data, len, mode, version, codewords);

  const struct ErrData err = get_err_data(version);

  uint8_t res_bits[MAX_CODEWORDS_BITS];

  // Write white bytes here so if the QR symbol has extra space white will be written
  for (uint16_t i = 0; i < sizeof(res_bits); i++) {
    res_bits[i] = 0;
  }

  get_full_codewords(err, codewords, res_bits);

  uint8_t qr_matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE] = {0};

  uint8_t qr_matrix_size = QR_MATRIX_SIZE(version.version);

  write_patterns(qr_matrix, version.version, qr_matrix_size);

  if (version.version >= MIN_VERSION_INFO_VERSION) {
    write_version_info(qr_matrix, version.version, qr_matrix_size);
  }

  write_format_info(qr_matrix, err_ver, qr_matrix_size);

  write_matrix(qr_matrix, res_bits, qr_matrix_size);

  uint8_t bmp[MAX_BMP_LEN] = {0};

  uint32_t bmp_len = matrix_to_bmp(bmp, qr_matrix, qr_matrix_size);

  syscall3(__NR_write, 1, (long)bmp, bmp_len);
}
