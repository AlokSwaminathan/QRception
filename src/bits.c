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
// Bytes are filled up from the LSB first
// Will always write <bit> bits
// Returns number of bits written
uint16_t write_bits(byte *arr, uint16_t curr_bit, uint32_t data, byte bits){
  arr += curr_bit / 8;
  byte offset = curr_bit % 8;
  byte existing_data = arr[0] & (((1 << offset) - 1) << (8 - offset));
  existing_data >>= (8 - offset);

  data &= (1 << bits) - 1;
  data += existing_data << bits;
  data = reverse_bits(data); 
  data >>= ((sizeof(data) * 8) - bits - offset);

  data = reverse_bytes(data);

  ((uint32_t*)arr)[0] = data;

  return bits;
}
