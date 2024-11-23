#include "constants.h"
#include "func_table.h"
#include "types.h"

// Sizes stores the different sizes based on the length of the bits indicating the length of the data in that mode
// 0 - Versions 1-9
// 1 - Versions 10-26
// 2 - Versions 27-40
void get_mode_specific_size(uint16_t sizes[3], const enum Mode mode, const uint16_t len) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
  const uint8_t *character_counts;
  uint16_t base_size = MODE_INDICATOR_LEN_BITS;

  // Calculate the different possible sizes of the data based on the encoding method
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

  // Use the varying sizes to find the size for all possible QR code version
  for (uint8_t i = 0; i < DISTINCT_CHARACTER_COUNT_SIZES; i++) {
    sizes[i] = base_size + character_counts[i];
    const uint8_t full_byte = sizes[i] % 8 == 0;
    sizes[i] /= 8;
    if (!full_byte)
      sizes[i] += 1;
  }

#pragma GCC diagnostic pop
}
