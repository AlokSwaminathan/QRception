#include "constants.h"
#include "func_table.h"
#include "types.h"

// Calculates size
uint16_t get_mode_size(const enum Mode mode, const uint16_t len) {
  uint16_t size = MODE_INDICATOR_LEN_BITS;

  // Calculate the different possible sizes of the data based on the encoding method
  switch (mode) {
  case NUM:
    size += 10 * (len / 3) + ((len % 3 == 1) ? 4 : 0) + ((len % 3 == 2) ? 7 : 0);
    size += NUMERIC_CHARACTER_COUNT_INDICATOR_LEN;
    break;
  case ALPH_NUM:
    size += 11 * (len / 2) + 6 * (len % 2);
    size += ALPHANUMERIC_CHARACTER_COUNT_INDICATOR_LEN;
    break;
  case BYTE:
    size += 8 * len;
    size += BYTE_CHARACTER_COUNT_INDICATOR_LEN;
    break;
  }

  const uint8_t full_byte = size % 8 == 0;
  size /= 8;
  if (!full_byte)
    size += 1;

  return size;
}
