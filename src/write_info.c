#include "constants.h"
#include "func_table.h"
#include "types.h"

void write_matrix(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t codewords_bits[MAX_CODEWORDS_BITS], const uint8_t version_size) {
  uint16_t curr_bit = 0;

  // Direction we are moving along y axis
  bool up = true;

  // Iterate through two x at a time since bits are ideally written in a 2x4 block
  for (uint8_t x = QR_MATRIX_PADDING + version_size - 1; x > QR_MATRIX_PADDING - 1; x -= 2) {
    // Sets diff to 255 if up=1 (true), otherwise diff is 1
    uint8_t diff = 1 + 254 * up;
    // 0 if true, version_size - 1 if false
    uint8_t start = up * (version_size - 1);

    uint8_t iters = 0;
    for (uint8_t y = QR_MATRIX_PADDING + start; iters < version_size; y += diff, iters++) {
      if (matrix[y][x] == QR_MATRIX_DEFAULT_VALUE) {
        // Write the final value for the BMP, not a QR matrix intermediate
        // 0 or 255
        // Also apply the relevant mask to the data
        matrix[y][x] = MASK_DATA(codewords_bits[curr_bit], x, y);
        curr_bit++;
      } else {
        // If this bit has been set already: patterns or version/format info
        // Update to BMP final value
        matrix[y][x] /= 2;
        matrix[y][x] += 255;
      }

      // Do the same for the other value of x since 2x4 block
      // Somehow saves space, idk why the compiler can't do this
      const uint8_t new_x = x - 1;
      if (matrix[y][new_x] == QR_MATRIX_DEFAULT_VALUE) {
        matrix[y][new_x] = MASK_DATA(codewords_bits[curr_bit], x - 1, y);
        curr_bit++;
      } else {
        matrix[y][new_x] /= 2;
        matrix[y][new_x] += 255;
      }
    }

    // If we are at the vertical timing pattern, then we have to skip that whole column since there are no bits we can write there, so we move past it
    x -= (x == (QR_MATRIX_PADDING + FINDER_PATTERN_HEIGHT - 1));

    // Reverse direction
    up = !up;
  }

  // Since we skipped the column with the vertical timing pattern we have to correct it here
  uint8_t x = QR_MATRIX_PADDING + FINDER_PATTERN_HEIGHT - 3;
  for (uint8_t y = QR_MATRIX_PADDING; y < version_size + QR_MATRIX_PADDING; y++) {
    matrix[y][x] /= 2;
    matrix[y][x] += 255;
  }
}

// This writes the version information for the QR code in the two designated places
void write_version_info(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const uint8_t version, const uint8_t version_size) {
  const uint32_t version_info = generate_golay_code(version + 1);

  // Named x and y, but they can swap because the version info is written in 2 places where the x and the y are swapped
  // Essentially the two locations are reflections across the diagonal line from the top left to the bottom right
  const uint8_t start_x = QR_MATRIX_PADDING + version_size - FINDER_PATTERN_HEIGHT - 2;
  uint8_t start_y = QR_MATRIX_PADDING;

  // The code is 18 bits long and is written in 6 strips of 3
  for (; start_y < (GOLAY_LEN / 3) + QR_MATRIX_PADDING; start_y++) {
    const uint8_t shift = (start_y - 3) * 3;

    // Calculate the 3 bits for this strip
    const uint8_t bit1 = BIT_TO_QR_MATRIX_VAL((version_info & (1 << shift)) > 0);
    const uint8_t bit2 = BIT_TO_QR_MATRIX_VAL((version_info & (1 << (shift + 1))) > 0);
    const uint8_t bit3 = BIT_TO_QR_MATRIX_VAL((version_info & (1 << (shift + 2))) > 0);

    matrix[start_y][start_x] = bit1;
    matrix[start_x][start_y] = bit1;
    matrix[start_y][start_x + 1] = bit2;
    matrix[start_x + 1][start_y] = bit2;
    matrix[start_y][start_x + 2] = bit3;
    matrix[start_x + 2][start_y] = bit3;
  }
}

// This writes the format information for the QR code in the two designated places
void write_format_info(uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], const enum ErrorCorrectionVersion ec, const uint8_t version_size) {
  // Merges error correction level and data mask into 5 bits of data
  // Generates the BCH code then masks it with a predetermined mask
  const uint16_t format_info = generate_bch_code((ec << 3) + DEFAULT_DATA_MASK) ^ FORMAT_INFO_MASK;

  // The const coord is the x value if we are writing the vertical format info
  // The const coord is the y value if we are writing the horizontal format info
  const uint8_t const_coord = QR_MATRIX_PADDING + FINDER_PATTERN_HEIGHT - 1;
  uint8_t vertical_coord = QR_MATRIX_PADDING;
  uint8_t horizontal_coord = QR_MATRIX_PADDING + version_size - 1;

  for (uint8_t shift = 0; shift < BCH_LEN; shift++) {
    const uint8_t bit = BIT_TO_QR_MATRIX_VAL((format_info & (1 << shift)) > 0);

    // Write to the correct spot on the two location
    matrix[vertical_coord][const_coord] = bit;
    matrix[const_coord][horizontal_coord] = bit;

    // Depending on the shift, we might have to jump over part of the QR code or skip over a special bit
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
