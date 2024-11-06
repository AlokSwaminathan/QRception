#include "func_table.h"
#include "constants.h"
#include "types.h"

void write_matrix(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE],uint8_t *codewords_bits, uint8_t version_size) {
  uint16_t curr_bit = 0;
  bool up = true;
  for (uint8_t x = QR_MATRIX_PADDING + version_size - 1; x > QR_MATRIX_PADDING - 1; x -= 2) {
    // Sets diff to 255 if up=1 (true), otherwise diff is 1
    uint8_t diff = 1 + 254*up;
    // 0 if true, version_size - 1 if false
    uint8_t start = up * (version_size - 1);
    uint8_t iters = 0;
    for (uint8_t y = QR_MATRIX_PADDING + start; iters < version_size; y += diff, iters++) {
      if (matrix[y][x] == QR_MATRIX_DEFAULT_VALUE) {
        // Write BMP final value now to save time
        matrix[y][x] = MASK_DATA(codewords_bits[curr_bit], y);
        curr_bit++;
      } else {
        // Update to BMP final value
        matrix[y][x] /= 2;
        matrix[y][x] += 255;
      }

      // Somehow saves space, idk why the compiler can't do this
      uint8_t new_x = x - 1;
      if (matrix[y][new_x] == QR_MATRIX_DEFAULT_VALUE) {
        matrix[y][new_x] = MASK_DATA(codewords_bits[curr_bit], y);
        curr_bit++;
      } else {
        matrix[y][new_x] /= 2;
        matrix[y][new_x] += 255;
      }
    }

    x -= (x == (QR_MATRIX_PADDING + FINDER_PATTERN_HEIGHT - 1));
  
    up = !up;
  }
  uint8_t x = QR_MATRIX_PADDING + FINDER_PATTERN_HEIGHT - 3;
  for (uint8_t y = QR_MATRIX_PADDING; y < version_size + QR_MATRIX_PADDING; y++) {
    matrix[y][x] /= 2;
    matrix[y][x] += 255;
  }
}

void write_version_info(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], uint8_t version, uint8_t version_size) {
  uint32_t version_info = generate_golay_code(version+1);
  uint8_t start_x = QR_MATRIX_PADDING + version_size - FINDER_PATTERN_HEIGHT - 2; 
  uint8_t start_y = QR_MATRIX_PADDING;
  for (; start_y < (GOLAY_LEN / 3) + QR_MATRIX_PADDING; start_y++) {
    uint8_t shift = (start_y - 3) * 3;
  
    uint8_t bit1 = BIT_TO_QR_MATRIX_VAL((version_info & (1 << shift)) > 0);
    uint8_t bit2 = BIT_TO_QR_MATRIX_VAL((version_info & (1 << (shift + 1))) > 0);
    uint8_t bit3 = BIT_TO_QR_MATRIX_VAL((version_info & (1 << (shift + 2))) > 0);

    matrix[start_y][start_x] = bit1;
    matrix[start_x][start_y] = bit1;
    matrix[start_y][start_x + 1] = bit2;
    matrix[start_x + 1][start_y] = bit2;
    matrix[start_y][start_x + 2] = bit3;
    matrix[start_x + 2][start_y] = bit3;
  }
}

void write_format_info(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], enum ErrorCorrectionVersion ec, uint8_t version_size) {
  uint16_t format_info = generate_bch_code((ec << 3) + DEFAULT_DATA_MASK) ^ FORMAT_INFO_MASK;
  
  // x for vertical, y for horizontal
  uint8_t const_coord = QR_MATRIX_PADDING + FINDER_PATTERN_HEIGHT - 1;
  uint8_t vertical_coord = QR_MATRIX_PADDING;
  uint8_t horizontal_coord = QR_MATRIX_PADDING + version_size - 1;

  for (uint8_t shift = 0; shift < BCH_LEN; shift++) {
    uint8_t bit = BIT_TO_QR_MATRIX_VAL((format_info & (1 << shift)) > 0);
    matrix[vertical_coord][const_coord] = bit;
    matrix[const_coord][horizontal_coord] = bit;
    if (shift == 7) {
      // One less than expected because we add/subtract one always at end of loop
      uint8_t change = version_size - BCH_LEN - 1;
      vertical_coord += change;
      horizontal_coord -= change;
    } else if (shift == 8) {
      horizontal_coord--;
    } else if (shift == 5) {
      vertical_coord++;
    }
    vertical_coord++;
    horizontal_coord--;
  }
}
