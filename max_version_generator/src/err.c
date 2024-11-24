#include "constants.h"
#include "func_table.h"
#include "types.h"

// Takes data and err and uses the data to calculate the error correction codewords
void rs_compute_codewords(const uint8_t *data, uint8_t data_len, uint8_t *err) {
  uint8_t res_poly[MAX_POLYNOMIAL_LEN] = {0};
  gf_poly_div(res_poly, data, data_len, RS_GEN_POLY, ERR_LEN + 1);
  for (uint8_t i = 0; i < ERR_LEN; i++) {
    err[i] = res_poly[data_len + i];
  }
}

// Takes the codewords and generates the error codewords
// Then it formats the data and error codewords in the format that the specification requires (not in normal order)
// Res is written to in a way such that every byte represents one bit so that it is easier to write to the matrix
void get_full_codewords(const uint8_t data[MAX_DATA_CODEWORDS], uint8_t res[MAX_CODEWORDS_BITS]) {
  uint8_t errs[256][256];

  // For every block in the first group calculate the error codewords
  uint16_t curr = 0;
  for (uint8_t i = 0; i < ERR_GROUP_ONE_LEN; i++) {
    rs_compute_codewords(&data[curr], ERR_BLOCK_ONE_DATA_LEN, errs[i]);
    curr += ERR_BLOCK_ONE_DATA_LEN;
  }

  // For every block in the second group calculate the error codewords
  for (uint8_t i = 0; i < ERR_GROUP_TWO_LEN; i++) {
    rs_compute_codewords(&data[curr], ERR_BLOCK_TWO_DATA_LEN, errs[i + ERR_GROUP_ONE_LEN]);
    curr += ERR_BLOCK_TWO_DATA_LEN;
  }

  // Write the data blocks to the final result in the final order as designated by the QR spec
  uint16_t curr_res = 0;
  for (uint8_t i = 0; i < ERR_BLOCK_TWO_DATA_LEN; i++) {
    curr = i;
    for (uint8_t j = 0; j < ERR_GROUP_ONE_LEN; j++) {
      // Multiply by truthy value if data is in range of the block, otherwise writing 0 and nothing happens
      curr_res += write_bits(res, curr_res, data[curr], 8 * (i < ERR_BLOCK_ONE_DATA_LEN));
      curr += ERR_BLOCK_ONE_DATA_LEN;
    }
    for (uint8_t j = 0; j < ERR_GROUP_TWO_LEN; j++) {
      curr_res += write_bits(res, curr_res, data[curr], 8);
      curr += ERR_BLOCK_TWO_DATA_LEN;
    }
  }

  // Write the error blocks to the final result in the final order as designated by the QR spec
  for (uint8_t i = 0; i < ERR_LEN; i++) {
    for (uint8_t j = 0; j < ERR_GROUP_ONE_LEN + ERR_GROUP_TWO_LEN; j++) {
      curr_res += write_bits(res, curr_res, errs[j][i], 8);
    }
  }
}
