#include "constants.h"
#include "func_table.h"
#include "types.h"
#include <stdint.h>

void write_rings(uint8_t x, uint8_t y, const uint8_t size, uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], uint8_t val, const uint8_t limit) {
  // Start from the outer ring, then write bytes starting from each corner
  // Results in some areas having a value written to them twice, but this is still most efficient code size wise afaik
  for (uint8_t j = 0; j < (size + 1) / 2 && j < limit; j++) { 
    uint8_t curr_size = size - 2*j;
    for (uint8_t i = 0; i < curr_size; i++){
      matrix[y+i][x+0] = val;
      matrix[y+0][x+i] = val;
      matrix[y+curr_size-1][x+i] = val;
      matrix[y+i][x+curr_size-1] = val;
    }
    
    // If the val is 255 (the value of the white boundary) then we dont want to reverse it
    // Otherwise we want to swap the value
    val = val < 255 ? REVERSE_QR_MATRIX_VAL(val) : 255;
    x++;
    y++;
  }
}

#define write_finder_pattern(x, y, matrix) { write_rings(x, y, 9, matrix, QR_MATRIX_WHITE_VALUE, 255); matrix[(y)+4][(x)+4] = QR_MATRIX_BLACK_VALUE;}
#define write_alignment_pattern(x, y, matrix) { write_rings(x, y, 5, matrix, QR_MATRIX_BLACK_VALUE, 255); }

// Returns the highest possible value of either coordinate for an alignment coord using a 0-indexed version
#define max_alignment_coord(version) (14 + 4*(version))

static inline void write_alignment_patterns(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t version) {
  if (version == 0)
    return;

  const uint8_t num_coords = ((version + 1) / 7) + 2;

  const uint8_t top_val = max_alignment_coord(version);

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

// Writes all of the patterns to the QR Matrix
// Finder patterns, timing patterns, and alignment patterns if they exist
void write_patterns(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t version, const uint8_t version_size) {
  // Write the 3 finder patterns
  write_finder_pattern(QR_MATRIX_PADDING-1,QR_MATRIX_PADDING-1,matrix);
  write_finder_pattern(QR_MATRIX_PADDING-1, QR_MATRIX_PADDING+version_size-FINDER_PATTERN_HEIGHT+1, matrix);
  write_finder_pattern(QR_MATRIX_PADDING+version_size-FINDER_PATTERN_HEIGHT+1, QR_MATRIX_PADDING-1, matrix);

  // BMP to paddings
  write_rings(0,0,version_size+2*QR_MATRIX_PADDING,matrix,255, QR_MATRIX_PADDING);

  // Set random black bit
  matrix[QR_MATRIX_PADDING+version_size-FINDER_PATTERN_HEIGHT+1][QR_MATRIX_PADDING+FINDER_PATTERN_HEIGHT-1] = QR_MATRIX_BLACK_VALUE;

  // Set timing patterns
  uint8_t timer_len = version_size - 2*(FINDER_PATTERN_HEIGHT-1);
  uint8_t timer_base = QR_MATRIX_PADDING + FINDER_PATTERN_HEIGHT - 3;
  uint8_t timer_val = QR_MATRIX_BLACK_VALUE;
  for (uint8_t i = 0; i < timer_len; i++) {
    matrix[timer_base][timer_base + 2 + i] = timer_val;
    matrix[timer_base + 2 + i][timer_base] = timer_val;
    timer_val = REVERSE_QR_MATRIX_VAL(timer_val);
  }

  // Write alignment patterns
  write_alignment_patterns(matrix,version);
}
