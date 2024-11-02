#include "func_table.h"
#include "types.h"
#include "constants.h"

// Assumes bmp is long enough
// Assumes bmp is 0'd
uint32_t matrix_to_bmp(uint8_t *bmp, uint8_t matrix[MAX_QR_MATRIX_SIZE][MAX_QR_MATRIX_SIZE], uint8_t version_size) {
  uint8_t *start = bmp;

  uint16_t pixel_width = BMP_WIDTH_HEIGHT(version_size);

  bmp[BMP_FILE_HEADER_OFFSET] = BMP_FILE_HEADER_ONE;
  bmp[BMP_FILE_HEADER_OFFSET + 1] = BMP_FILE_HEADER_TWO;
  
  bmp[BMP_DATA_OFFSET_OFFSET] = BMP_HEADERS_LEN;

  bmp[BMP_SECOND_HEADER_SIZE_OFFSET] = BMP_SECOND_HEADER_SIZE;

  *(uint16_t*)(&bmp[BMP_WIDTH_PIXELS_OFFSET]) = pixel_width;
  *(uint16_t*)(&bmp[BMP_HEIGHT_PIXELS_OFFSET]) = pixel_width;

  bmp[BMP_COLOR_PLANES_OFFSET] = BMP_NUM_COLOR_PLANES;
  bmp[BMP_BITS_PER_PIXEL_OFFSET] = BMP_BITS_PER_PIXEL;

  *(uint16_t*)(&bmp[BMP_HORIZONTAL_RES_OFFSET]) = BMP_PIXELS_PER_METER;
  *(uint16_t*)(&bmp[BMP_VERTICAL_RES_OFFSET]) = BMP_PIXELS_PER_METER;

  bmp[BMP_WHITE_COLOR_OFFSET] = BMP_WHITE_VAL;
  bmp[BMP_WHITE_COLOR_OFFSET + 1] = BMP_WHITE_VAL;
  bmp[BMP_WHITE_COLOR_OFFSET + 2] = BMP_WHITE_VAL;

  bmp += BMP_HEADERS_LEN;
  uint8_t max = version_size + (2 * QR_MATRIX_PADDING);
  for (uint8_t y = max - 1; y < 255; y--) {
    for (uint8_t i = 0; i < BMP_PIXELS_PER_MODULE; i++) {
      for (uint8_t x = 0; x < max; x++) {
        *(bmp++) = matrix[y][x];
      }
      // Pad each row to 4 bytes, only works since padding is 3 right now
      // Need to change this if the padding changes
      // Already 0'd out so we can just increment
      bmp++;
    }
  }

  uint32_t total_len = bmp - start;

  *(uint32_t*)(&bmp[BMP_FILE_SIZE_OFFSET]) = total_len;

  return total_len; 
}
