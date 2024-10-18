#include "func_table.h"
#include "types.h"
#include "constants.h"

void write_err_block(uint8_t* data, uint8_t data_len, uint8_t* err_codewords, uint8_t* err_codewords_len) {
  uint8_t b[MAX_ERROR_CODEWORDS];
  
}

struct ErrData get_err_data(struct Version version) {
  uint16_t total_block_cw = ERR_CODEWORD_BLOCKS[version.version][version.ec_version];

  uint8_t block1_groups = (version.cw_capacity / total_block_cw) - (version.cw_capacity % total_block_cw);
  uint8_t block2_groups = (version.cw_capacity % total_block_cw);
  
  if (block2_groups > 0 && block1_groups > total_block_cw + 1) {
    block1_groups -= total_block_cw + 1;
    block2_groups -= total_block_cw;
  }

  uint8_t data1_len = (version.capacity - block2_groups) / (block1_groups + block2_groups);
  uint8_t data2_len = data1_len + 1;

  uint8_t err_len = total_block_cw - data1_len;

  return (struct ErrData) {
    .block_lens = {block1_groups, block2_groups},
    .data_lens = {data1_len, data2_len},
    .err_len = err_len,
  };
}

// res should be at least degree long
// res should be 0'd
void rs_gen_poly(uint8_t degree, uint8_t *res) {
  res[0] = 1;
  uint8_t basic_poly[2] = {1, 0};
  for (uint8_t i = 0; i < degree; i++){
    basic_poly[1] = gf_pow(2, i);
    uint8_t res2[MAX_POLYNOMIAL_LEN] = {0};
    gf_poly_mul(res2, res, i+1, basic_poly, 2);
    for (uint8_t j = 0; j < 2 + i; j++) {
      res[j] = res2[j];
    }
  }
}

void rs_compute_codewords(uint8_t *data, uint8_t data_len, uint8_t *err, uint8_t err_len) {
  uint8_t gen_poly[MAX_POLYNOMIAL_LEN] = {0};
  uint8_t res_poly[MAX_POLYNOMIAL_LEN] = {0};
  rs_gen_poly(err_len, gen_poly);
  gf_poly_div(res_poly, data, data_len, gen_poly, err_len + 1); 
  for (uint8_t i = 0; i < err_len; i++) {
    err[i] = res_poly[data_len+i];
  }
}
