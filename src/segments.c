#include "types.h"
#include "func_table.h"
#include "constants.h"

// Expects sizes to be a long[3] with the sizes depending on QR version
// 0 - Versions 1-9
// 1 - Versions 10-26
// 2 - Versions 27-40
void get_mode_specific_size(uint16_t *sizes, enum Mode mode, uint16_t len) {
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
  const uint8_t *character_counts;
  uint16_t base_size = MODE_INDICATOR_LEN_BITS;
  switch (mode) {
  case NUM:
    character_counts = NUMERIC_CHARACTER_COUNT_LEN;
    base_size += 10 * (len / 3) + ((len % 3 == 1) ? 4 : 0) + ((len % 3 == 2) ? 7 : 0);
    break;
  case ALPH_NUM:
    character_counts = ALPHANUMERIC_CHARACTER_COUNT_LEN;
    base_size += 11 * (len / 2) + 6 * (len % 2);
    break;
  case BYTE:
    character_counts = BYTE_CHARACTER_COUNT_LEN;
    base_size += 8 * len;
    break;
  }
  for (uint8_t i = 0; i < DISTINCT_CHARACTER_COUNT_SIZES; i++) {
    sizes[i] = base_size + character_counts[i];
    sizes[i] /= 8;
  }
  #pragma GCC diagnostic pop
}
