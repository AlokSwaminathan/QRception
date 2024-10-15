#include "types.h"

static inline uint32_t reverse_bits(uint32_t data) {
  unsigned int reversed = 0;
  for (int i = 0; i < sizeof(data) * 8; i++){
    reversed |= ((data & (1 << i)) > 0) << ((63 - i) % 32);
  }
  return reversed;
}

// Supports writes up to 16 bits, since that is the largest that will be needed
// Writes data in big endian, so MSB comes before LSB (This is just easier for me to think about)
// Bytes are also big endian individually
// Will always write <bit> bits
// Offset should be 0-7
void write_bits(byte *arr, byte offset, uint32_t data, byte bits){
  byte existing_data = arr[0] & ((1 << offset) - 1);

  data &= (1 << bits) - 1;
  data += existing_data << bits;

  data = reverse_bits(data);
  data >>= (sizeof(data) * 8) - bits - offset;

  ((uint32_t*)arr)[0] = data;
}
