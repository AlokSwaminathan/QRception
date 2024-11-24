#include "constants.h"
#include "func_table.h"
#include "syscalls.h"
#include "types.h"

// Returns new curr bit
uint16_t encode_bytes(const uint8_t data[MAX_DATA_CODEWORDS], const uint16_t data_len, uint8_t codewords[MAX_DATA_CODEWORDS], uint16_t curr_bit) {
  for (uint16_t i = 0; i < data_len; i++) {
    curr_bit += write_bits(codewords, curr_bit, (uint32_t)data[i], BYTE_LEN_BITS);
  }
  return curr_bit;
}

// Returns new curr bit
uint16_t encode_alphanumeric(const uint8_t data[MAX_DATA_CODEWORDS], const uint16_t data_len, uint8_t codewords[MAX_DATA_CODEWORDS], uint16_t curr_bit) {
  uint16_t alph_data = 0;
  uint8_t bits = ALPHANUMERIC_ONE_LEN_BITS;
  const uint8_t incr = ALPHANUMERIC_TWO_LEN_BITS - ALPHANUMERIC_ONE_LEN_BITS;
  for (uint16_t i = 0; i < data_len; i++) {
    alph_data = 45 * alph_data + ascii_to_alphanumeric(data[i]);
    if (bits == ALPHANUMERIC_TWO_LEN_BITS) {
      curr_bit += write_bits(codewords, curr_bit, alph_data, ALPHANUMERIC_TWO_LEN_BITS);
      alph_data = 0;
      bits = ALPHANUMERIC_ONE_LEN_BITS - incr;
    }
    bits += incr;
  }
  if (data_len % 2) {
    curr_bit += write_bits(codewords, curr_bit, alph_data, bits - incr);
  }
  return curr_bit;
}

// Returns new curr bit
uint16_t encode_numeric(const uint8_t data[MAX_DATA_CODEWORDS], const uint16_t data_len, uint8_t codewords[MAX_DATA_CODEWORDS], uint16_t curr_bit) {
  uint16_t numeric_data = 0;
  uint8_t bits = NUMERIC_ONE_LEN_BITS;
  for (uint16_t i = 0; i < data_len; i++) {
    numeric_data = (numeric_data * 10) + (data[i] - '0');
    if (bits == NUMERIC_THREE_LEN_BITS) {
      curr_bit += write_bits(codewords, curr_bit, numeric_data, bits);
      numeric_data = 0;
      bits = NUMERIC_ONE_LEN_BITS - 3;
    }
    bits += 3;
  }
  if (numeric_data) {
    curr_bit += write_bits(codewords, curr_bit, numeric_data, bits - 3);
  }
  return curr_bit;
}

// Expects codewords to be len 2956
// Version should be 1-3 in this case, representing a character count version
void encode_into_codewords(const uint8_t data[MAX_DATA_CODEWORDS], const uint16_t data_len, const enum Mode mode, uint8_t codewords[MAX_DATA_CODEWORDS]) {
  // Current bit of codewords
  uint8_t extended_codewords[MAX_CODEWORDS * 8];
  uint16_t curr_bit = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

  uint8_t cc_len_bits;
  uint8_t mode_indicator;
  ModeEncoder encoder;

  switch (mode) {
  case NUM:
    encoder = encode_numeric;
    mode_indicator = NUMERIC_MODE_INDICATOR;
    cc_len_bits = NUMERIC_CHARACTER_COUNT_INDICATOR_LEN;
    break;
  case ALPH_NUM:
    encoder = encode_alphanumeric;
    mode_indicator = ALPHANUMERIC_MODE_INDICATOR;
    cc_len_bits = ALPHANUMERIC_CHARACTER_COUNT_INDICATOR_LEN;
    break;
  case BYTE:
    encoder = encode_bytes;
    mode_indicator = BYTE_MODE_INDICATOR;
    cc_len_bits = BYTE_CHARACTER_COUNT_INDICATOR_LEN;
    break;
  }

  // Writes mandatory mode indicator and size
  curr_bit += write_bits(extended_codewords, curr_bit, (uint32_t)mode_indicator, MODE_INDICATOR_LEN_BITS);
  curr_bit += write_bits(extended_codewords, curr_bit, (uint32_t)data_len, cc_len_bits);

  // Writes the actual data, encoded based on mode
  curr_bit = encoder(data, data_len, extended_codewords, curr_bit);

#pragma GCC diagnostic pop

  // Write mandatory terminator
  curr_bit += write_bits(extended_codewords, curr_bit, TERMINATOR, MODE_INDICATOR_LEN_BITS);

  // Extra bits to ensure everything is rounded to a byte
  curr_bit += write_bits(extended_codewords, curr_bit, 0, 8);

  // Merge extended_codewords into normal one
  curr_bit /= 8;
  for (uint16_t i = 0; i < curr_bit; i++) {
    uint8_t byte = 0;
    for (uint8_t j = 0; j < 8; j++) {
      byte = (byte << 1) + extended_codewords[8 * i + j];
    }
    codewords[i] = byte;
  }

  // If there is any extra space, write the padding bits
  for (; curr_bit < DATA_CODEWORDS; curr_bit += 2) {
    *(uint16_t *)(&codewords[curr_bit]) = 0b0001000111101100;
  }
}
