#include "types.h"

static inline uint32_t reverse_bits(uint32_t data) {
  uint32_t reversed = 0;
  for (uint8_t i = 0; i < sizeof(data) * 8; i++){
    reversed |= ((data & (1 << i)) > 0) << ((63 - i) % 32);
  }
  return reversed;
}

static inline uint32_t reverse_bytes(uint32_t data) {
  uint32_t rev_bytes = 0;
  for (uint8_t i = 0; i < sizeof(data); i++) {
    rev_bytes >>= 8;
    uint32_t byte_mask = ((1 << 8) - 1) << (8*i);
    rev_bytes |= reverse_bits(data & byte_mask) << (8*i);
  }
  return rev_bytes;
}

// Supports writes up to 16 bits, since that is the largest that will be needed
// Writes data in big endian, so MSB comes before LSB (This is just easier for me to think about)
// Will always write <bit> bits
// Returns number of bits written
// One byte per bit
uint16_t write_bits(byte *arr, uint16_t curr_bit, uint16_t data, byte bits){
  for (uint8_t i = 0; i < bits; i++) {
    arr[curr_bit+(bits - i)-1] = data & (1 << i) ? 1 : 0;
  }
  return bits;
}
