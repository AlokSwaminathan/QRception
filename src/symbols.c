#include "constants.h"
#include "func_table.h"
#include "types.h"
#include <stdint.h>

void write_rings(uint8_t x, uint8_t y, uint8_t size, uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], uint8_t val) {
  for (uint8_t j = 0; j < (size + 1) / 2; j++) { 
    uint8_t curr_size = size - 2*j;
    for (uint8_t i = 0; i < curr_size; i++){
      matrix[y+i][x+0] = val;
      matrix[y+0][x+i] = val;
      matrix[y+curr_size-1][x+i] = val;
      matrix[y+i][x+curr_size-1] = val;
    }
    val = (!(val-1))+1;
    x++;
    y++;
  }
}

#define write_finder_pattern(x, y, matrix) { write_rings(x, y, 9, matrix, QR_MATRIX_WHITE_VALUE); matrix[(y)+4][(x)+4] = QR_MATRIX_BLACK_VALUE;}
#define write_alignment_pattern(x, y, matrix) { write_rings(x, y, 5, matrix, QR_MATRIX_BLACK_VALUE); }

#define max_alignment_coord(version) (14 + 4*(version))

void write_patterns(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], uint8_t version, uint8_t version_size) {
  // Write the 3 finder patterns
  write_finder_pattern(QR_MATRIX_PADDING-1,QR_MATRIX_PADDING-1,matrix);
  write_finder_pattern(QR_MATRIX_PADDING-1, QR_MATRIX_PADDING+version_size-FINDER_PATTERN_HEIGHT+1, matrix);
  write_finder_pattern(QR_MATRIX_PADDING+version_size-FINDER_PATTERN_HEIGHT+1, QR_MATRIX_PADDING-1, matrix);

  // Set random black bit
  matrix[QR_MATRIX_PADDING+version_size-FINDER_PATTERN_HEIGHT+1][QR_MATRIX_PADDING+FINDER_PATTERN_HEIGHT-1] = QR_MATRIX_BLACK_VALUE;

  // Set timing patterns
  uint8_t timer_len = version_size - 2*(FINDER_PATTERN_HEIGHT-1);
  uint8_t timer_base = QR_MATRIX_PADDING + FINDER_PATTERN_HEIGHT - 3;
  uint8_t timer_val = 1;
  for (uint8_t i = 0; i < timer_len; i++) {
    matrix[timer_base][timer_base + 2 + i] = timer_val + 1;
    matrix[timer_base + 2 + i][timer_base] = timer_val + 1;
    timer_val = !timer_val;
  }

  // Write alignment patterns
  if (version == 0)
    return;

  uint8_t num_coords = ((version + 1) / 7) + 2;

  uint8_t top_val = max_alignment_coord(version);

  uint8_t x = top_val;
  for (uint8_t i = 0; i < num_coords; i++) {
    if (i == num_coords - 1)
      x = 6;
    uint8_t y = top_val;
    for (uint8_t j = 0; j < num_coords; j++) {
      if (j == num_coords - 1)
        y = 6;
      if ((x == 6 && (y == 6 || j == 0)) || (i == 0 && y == 6)){
        y -= ALIGNMENT_PATTERN_DIFFS[version]; 
        continue;
      }
      write_alignment_pattern(QR_MATRIX_PADDING + x - 2, QR_MATRIX_PADDING + y - 2, matrix);
      y -= ALIGNMENT_PATTERN_DIFFS[version]; 
    }
    x -= ALIGNMENT_PATTERN_DIFFS[version];
  }
}
