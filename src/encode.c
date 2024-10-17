#include "constants.h"
#include "func_table.h"
#include "types.h"

// Returns new curr bit
uint16_t encode_bytes(byte *data, uint16_t data_len, byte *codewords, uint16_t curr_bit) {
  for (uint16_t i = 0; i < data_len; i++){
    curr_bit += write_bits(codewords, curr_bit, (uint32_t) data[i], BYTE_LEN_BITS); 
  }
  return curr_bit;
}

// Returns new curr bit
uint16_t encode_alphanumeric(byte *data, uint16_t data_len, byte *codewords, uint16_t curr_bit) {
  uint16_t alph_data = 0;
  uint8_t bits = ALPHANUMERIC_ONE_LEN_BITS;
  uint8_t incr = ALPHANUMERIC_TWO_LEN_BITS - ALPHANUMERIC_ONE_LEN_BITS;
  for (uint16_t i = 0; i < data_len; i++){
    alph_data = 45*alph_data + ascii_to_alphanumeric(data[i]);
    if (bits == ALPHANUMERIC_TWO_LEN_BITS) {
      curr_bit += write_bits(codewords, curr_bit, alph_data, ALPHANUMERIC_TWO_LEN_BITS);
      alph_data = 0;
      bits = ALPHANUMERIC_ONE_LEN_BITS - incr;
    }
    bits += incr;
  }
  if (alph_data) {
    curr_bit += write_bits(codewords, curr_bit, alph_data, bits - incr);
  }
  return curr_bit;
}

// Returns new curr bit
uint16_t encode_numeric(byte *data, uint16_t data_len, byte *codewords, uint16_t curr_bit) {
  uint16_t numeric_data = 0;
  uint8_t bits = NUMERIC_ONE_LEN_BITS;
  for (uint16_t i = 0; i < data_len; i++){
    numeric_data = (numeric_data * 10) + (data[i] - '0');
    if (bits == NUMERIC_THREE_LEN_BITS) {
      curr_bit += write_bits(codewords,curr_bit,numeric_data,bits);
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
// Returns number of bytes written (last byte padded to 0)
// Version should be 1-3 in this case, representing a character count version
// Returns number of bytes written
uint16_t encode_into_codewords(byte *data, uint16_t data_len, byte *codewords, struct ModeSegment *segments, uint16_t segments_len, enum CharCountVersion cc_version) {
  // Current bit of codewords
  byte extended_codewords[MAX_CODEWORDS * 8];
  uint16_t curr_bit = 0;
  struct ModeSegment seg;
  // Character count and mode indicator data
  uint8_t cc_len_bits;
  uint8_t mode_indicator;
  ModeEncoder encoder;
  for (uint16_t i = 0; i < segments_len; i++){
    seg = segments[i]; 
    switch (seg.mode) {
    case NUM:
      encoder = encode_numeric;
      mode_indicator = NUMERIC_MODE_INDICATOR;
      cc_len_bits = NUMERIC_CHARACTER_COUNT_LEN[cc_version];
    break;
    case ALPH_NUM:
      encoder = encode_alphanumeric;
      mode_indicator = ALPHANUMERIC_MODE_INDICATOR;
      cc_len_bits = ALPHANUMERIC_CHARACTER_COUNT_LEN[cc_version];
    break;
    case BYTE:
      encoder = encode_bytes;
      mode_indicator = BYTE_MODE_INDICATOR;
      cc_len_bits = BYTE_CHARACTER_COUNT_LEN[cc_version];
    break;
    }
    curr_bit += write_bits(extended_codewords, curr_bit, (uint32_t) mode_indicator, MODE_INDICATOR_LEN_BITS);
    curr_bit += write_bits(extended_codewords, curr_bit, (uint32_t) seg.len, cc_len_bits);
    curr_bit = encoder(data, seg.len, extended_codewords, curr_bit);
    data += seg.len;
  }
  if (curr_bit % 8 > 0){
    curr_bit += write_bits(extended_codewords, curr_bit,0, 8);
  }
  // Merge extended_codewords into normal one
  for (uint16_t i = 0; i < curr_bit / 8; i++){
    uint8_t byte = 0;
    for (uint8_t j = 0; j < 8; j++){
      byte = (byte << 1) + extended_codewords[8*i+j];
    }
    codewords[i] = byte;
  }
  return curr_bit / 8;
}
