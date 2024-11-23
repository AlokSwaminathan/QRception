#include "constants.h"
#include "func_table.h"
#include "types.h"

// Uses data in constants to extract the rest of the error data
struct ErrData get_err_data(const struct Version version) {
  const uint16_t total_block_cw = ERR_CODEWORD_BLOCKS[version.version][version.ec_version];

  // Calculating the number of groups for each block of errors
  uint8_t block1_groups = (version.cw_capacity / total_block_cw) - (version.cw_capacity % total_block_cw);
  uint8_t block2_groups = (version.cw_capacity % total_block_cw);

  // Recalculating the block groups if they are too large
  if (block2_groups > 0 && block1_groups > total_block_cw + 1) {
    block1_groups -= total_block_cw + 1;
    block2_groups -= total_block_cw;
  }

  // Find the length of the data codewords in the first block
  const uint8_t data1_len = (version.capacity - block2_groups) / (block1_groups + block2_groups);

  // Length of error len, is the same for both blocks
  const uint8_t err_len = total_block_cw - data1_len;

  return (struct ErrData){
      .block_lens = {block1_groups, block2_groups},
      // This +1 pattern is always true
      .data_lens = {data1_len, data1_len + 1},
      .err_len = err_len,
  };
}

// Creates the reed solomon generator polynomial of a given degree
// res should be at least degree long
// res should be 0'd
void rs_gen_poly(const uint8_t degree, uint8_t res[MAX_POLYNOMIAL_LEN]) {
  res[0] = 1;
  for (uint8_t i = 0; i < degree; i++) {
    uint8_t res2[MAX_POLYNOMIAL_LEN] = {0};
    gf_poly_mul(res2, res, i + 1, gf_pow(2, i));
    for (uint8_t j = 0; j < 2 + i; j++) {
      res[j] = res2[j];
    }
  }
}

// Takes data and err and uses the data to calculate the error correction codewords
void rs_compute_codewords(const uint8_t *data, uint8_t data_len, uint8_t *err, uint8_t err_len) {
  uint8_t gen_poly[MAX_POLYNOMIAL_LEN] = {0};
  uint8_t res_poly[MAX_POLYNOMIAL_LEN] = {0};
  rs_gen_poly(err_len, gen_poly);
  gf_poly_div(res_poly, data, data_len, gen_poly, err_len + 1);
  for (uint8_t i = 0; i < err_len; i++) {
    err[i] = res_poly[data_len + i];
  }
}

// Takes the codewords and generates the error codewords
// Then it formats the data and error codewords in the format that the specification requires (not in normal order)
// Res is written to in a way such that every byte represents one bit so that it is easier to write to the matrix
void get_full_codewords(const struct ErrData err_data, const uint8_t data[MAX_DATA_CODEWORDS], uint8_t res[MAX_CODEWORDS_BITS]) {
  uint8_t errs[256][256];

  // For every block in the first group calculate the error codewords
  uint16_t curr = 0;
  for (uint8_t i = 0; i < err_data.block_lens[0]; i++) {
    rs_compute_codewords(&data[curr], err_data.data_lens[0], errs[i], err_data.err_len);
    curr += err_data.data_lens[0];
  }

  // For every block in the second group calculate the error codewords
  for (uint8_t i = 0; i < err_data.block_lens[1]; i++) {
    rs_compute_codewords(&data[curr], err_data.data_lens[1], errs[i + err_data.block_lens[0]], err_data.err_len);
    curr += err_data.data_lens[1];
  }

  // Write the data blocks to the final result in the final order as designated by the QR spec
  uint16_t curr_res = 0;
  for (uint8_t i = 0; i < err_data.data_lens[1]; i++) {
    curr = i;
    for (uint8_t j = 0; j < err_data.block_lens[0]; j++) {
      // Multiply by truthy value if data is in range of the block, otherwise writing 0 and nothing happens
      curr_res += write_bits(res, curr_res, data[curr], 8 * (i < err_data.data_lens[0]));
      curr += err_data.data_lens[0];
    }
    for (uint8_t j = 0; j < err_data.block_lens[1]; j++) {
      curr_res += write_bits(res, curr_res, data[curr], 8);
      curr += err_data.data_lens[1];
    }
  }

  // Write the error blocks to the final result in the final order as designated by the QR spec
  for (uint8_t i = 0; i < err_data.err_len; i++) {
    for (uint8_t j = 0; j < err_data.block_lens[0] + err_data.block_lens[1]; j++) {
      curr_res += write_bits(res, curr_res, errs[j][i], 8);
    }
  }
}

// Function to generate the (15,5) BCH codeword for the format information
// Returns integer with data bits before error bits
uint16_t generate_bch_code(const uint8_t data) {
  uint16_t codeword = (uint16_t)data << 10;
  uint16_t gen_poly = BCH_GEN_POLY;

  for (uint8_t i = 14; i >= 10; i--) {
    if (codeword & (1 << i)) {
      codeword ^= gen_poly << (i - 10);
    }
  }

  return (data << 10) | (codeword & 0x3FF);
}

// Function to generate the (18,6) Golay code for the version information
// Returns integer with data bits before error bits
uint32_t generate_golay_code(const uint8_t data) {
  uint32_t codeword = (uint32_t)data << 12;
  uint32_t gen_poly = GOLAY_GEN_POLY;

  for (uint8_t i = 17; i >= 12; i--) {
    if (codeword & (1 << i)) {
      codeword ^= gen_poly << (i - 12);
    }
  }

  return (data << 12) | (codeword & 0xFFF);
}
