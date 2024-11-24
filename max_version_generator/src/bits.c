#include "types.h"

// Supports writes up to 16 bits, since that is the largest that will be needed
// Writes data in big endian, so MSB comes before LSB (This is how the bit
// stream should be in QR Codes) Will always write <bit> bits Returns number of
// bits written Writes to an array that stores one bit in each index
uint16_t write_bits(uint8_t *arr, const uint16_t curr_bit, const uint16_t data, const uint8_t bits) {
  for (uint8_t i = 0; i < bits; i++) {
    arr[curr_bit + (bits - i) - 1] = data & (1 << i) ? 1 : 0;
  }
  return bits;
}
