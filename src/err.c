#include "func_table.h"
#include "types.h"
#include "constants.h"

void write_err_block(uint8_t* data, uint8_t data_len, uint8_t* err_codewords, uint8_t* err_codewords_len) {
  uint8_t b[MAX_ERROR_CODEWORDS];
  
}

struct ErrData get_err_data(struct Version version) {
  uint16_t total_cw = ERR_CODEWORD_BLOCKS[version.version][version.ec_version];
  
  struct ErrData res = {0};
  if (version.cw_capacity % total_cw == 0){
    res.block_lens[0] = version.cw_capacity / total_cw;
    res.blocks[0].data_len = version.capacity / res.block_lens[0];
    res.blocks[0].err_len = (version.cw_capacity - version.capacity) / res.block_lens[0];
    return res;
  }

  uint16_t num2 = total_cw + 1;

  for (uint8_t i = 0; i < version.cw_capacity / total_cw; i++){
    if ((version.cw_capacity - (i * total_cw)) % num2 == 0){
      res.block_lens[0] = i;
      res.block_lens[1] = (version.cw_capacity - (i * total_cw)) / num2;
      break;
    }
  }

  // a = version.capacity
  // c = res.block_lens[0]
  // d = res.block_lens[1]
  uint8_t data_cw_lower = (version.capacity - res.block_lens[1]) / (res.block_lens[0] + res.block_lens[1]);
  res.blocks[0].data_len = data_cw_lower;
  res.blocks[1].data_len = data_cw_lower + 1;
  res.blocks[0].err_len = total_cw - data_cw_lower;
  res.blocks[1].err_len = total_cw - data_cw_lower;

  return res;
}
