#include "constants.h"
#include "func_table.h"
#include "types.h"

void write_rings(uint8_t x, uint8_t y, uint8_t size, uint8_t top_right[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], uint8_t val) {
  for (uint8_t j = 0; j < (size + 1) / 2; j++) { 
    uint8_t curr_size = size - 2*j;
    for (uint8_t i = 0; i < curr_size; i++){
      top_right[x+i][y+0] = val;
      top_right[x+0][y+i] = val;
      top_right[x+curr_size-1][y+i] = val;
      top_right[x+i][y+curr_size-1] = val;
    }
    val = (~(val-1))+1;
  }
}

#define write_finder_pattern(x, y, top_right) { write_rings(x, y, 7, top_right, QR_MATRIX_WHITE_VALUE); top_right[x+3][y+3] = QR_MATRIX_BLACK_VALUE;}
#define write_alignment_pattern(x, y, top_right) { write_rings(x, y, 5, top_right, QR_MATRIX_BLACK_VALUE); }
