#include "types.h"

static inline uint32_t reverse_bits(uint32_t data) {
  unsigned int reversed = 0;
  for (uint8_t i = 0; i < sizeof(data) * 8; i++){
    reversed |= ((data & (1 << i)) > 0) << ((63 - i) % 32);
  }
  return reversed;
}

// Supports writes up to 16 bits, since that is the largest that will be needed
// Writes data in big endian, so MSB comes before LSB (This is just easier for me to think about)
// Bytes are filled up from the LSB first
// Will always write <bit> bits
// Returns number of bits written
uint16_t write_bits(byte *arr, uint16_t curr_bit, uint32_t data, byte bits){
  arr += curr_bit / 8;
  byte offset = curr_bit % 8;
  byte existing_data = arr[0] & ((1 << offset) - 1);

  data &= (1 << bits) - 1;
  data = reverse_bits(data); 
  data >>= ((sizeof(data) * 8) - bits - offset);

  data += existing_data;

  ((uint32_t*)arr)[0] = data;

  return bits;
}
