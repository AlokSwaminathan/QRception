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
