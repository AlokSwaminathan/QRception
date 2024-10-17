#include "constants.h"
#include "func_table.h"
#include "types.h"

// Expects sizes to be a long[3] with the sizes depending on QR version
// 0 - Versions 1-9
// 1 - Versions 10-26
// 2 - Versions 27-40
void get_mode_specific_size(uint16_t *sizes, enum Mode mode, uint16_t len) {
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
  }
}

enum SwitchMode should_switch(enum Mode curr_mode, enum Mode new_mode, uint16_t match_streak, uint16_t miss_streak) {
  if (new_mode > curr_mode) {
    bool promote = false;
    if (curr_mode == NUM && new_mode == BYTE)
      promote = match_streak < NUMERIC_BEFORE_BYTES_MIN;
    else if (curr_mode == NUM && new_mode == ALPH_NUM)
      promote = match_streak < NUMERIC_BEFORE_ALPHANUMERIC_MIN;
    else if (curr_mode == ALPH_NUM && new_mode == BYTE)
      promote = match_streak < ALPHANUMERIC_BEFORE_BYTES_MIN;
    return promote ? PROMOTE : NO_SWITCH;
  }
  bool demote = false;
  if (curr_mode == BYTE && new_mode == ALPH_NUM)
    demote = miss_streak >= BYTES_TO_ALPHANUMERIC_MIN;
  else if (curr_mode == BYTE && new_mode == NUM)
    demote = miss_streak >= BYTES_TO_NUMERIC_MIN;
  else if (curr_mode == ALPH_NUM && new_mode == NUM)
    demote = miss_streak >= ALPHANUMERIC_TO_NUMERIC_MIN;
  return demote ? DEMOTE : NO_SWITCH;
}

// Expects sizes to be long[3] with the sizes depending on QR version
// 0 - Versions 1-9
// 1 - Versions 10-26
// 2 - Versions 27-40
// Returns number of segments
uint16_t calculate_total_size_and_get_switches(uint16_t *sizes, byte *data, uint16_t len, struct ModeSegment *segments) {
  sizes[0] = 0;
  sizes[1] = 0;
  sizes[2] = 0;
  uint16_t old_sizes[3];
  byte *curr = data;
  enum Mode curr_mode = get_mode(*curr);
  uint16_t match_streak = 1;
  uint16_t miss_streak = 0;
  uint16_t switches = 0;
  for (uint16_t i = 1; i < len; i++) {
    enum Mode mode = get_mode(data[i]);
    if (mode == curr_mode) {
      match_streak += miss_streak + 1;
      miss_streak = 0;
    } else {
      miss_streak++;
      enum SwitchMode switch_status = should_switch(curr_mode, mode, match_streak, miss_streak);
      switch (switch_status) {
      case DEMOTE:
        get_mode_specific_size(old_sizes, curr_mode, match_streak);
        for (int j = 0; j < DISTINCT_CHARACTER_COUNT_SIZES; j++) {
          sizes[j] += old_sizes[j];
        }
        segments[switches] = (struct ModeSegment){
            .mode = curr_mode,
            .len = match_streak
        };
        switches++;
        curr_mode = mode;
        match_streak = miss_streak;
        miss_streak = 0;
        break;
      case PROMOTE:
        curr_mode = mode;
        match_streak += miss_streak;
        miss_streak = 0;
        break;
      case NO_SWITCH:
        break;
      }
    }
  }
  get_mode_specific_size(old_sizes, curr_mode, match_streak);
  for (uint16_t i = 0; i < DISTINCT_CHARACTER_COUNT_SIZES; i++) {
    sizes[i] += old_sizes[i];
  }
  segments[switches] = (struct ModeSegment){
    .mode = curr_mode,
    .len = match_streak
  };
  switches++;
  return switches;
}

// Returns number of bits written
uint16_t encode_bytes(byte *data, uint16_t data_len, byte *codewords, uint16_t curr_bit) {
  for (uint16_t i = 0; i < data_len; i++){
    curr_bit += write_bits(codewords, curr_bit, (uint32_t) data[i], BYTE_LEN_BITS); 
  }
  return curr_bit;
}

// Returns number of bits written
uint16_t encode_alphanumeric(byte *data, uint16_t data_len, byte *codewords, uint16_t curr_bit) {
  uint32_t alph_data;
  for (uint16_t i = 0; i < data_len - 2; i += 2){
    alph_data = 45*ascii_to_alphanumeric(data[i]);
    alph_data += ascii_to_alphanumeric(data[i+1]);
    curr_bit += write_bits(codewords, curr_bit, alph_data, ALPHANUMERIC_TWO_LEN_BITS);
  }
  if (data_len % 2 == 1){
    alph_data = ascii_to_alphanumeric(data[data_len - 1]);
    curr_bit += write_bits(codewords, curr_bit, alph_data, ALPHANUMERIC_ONE_LEN_BITS);
  } else{
    alph_data = 45*ascii_to_alphanumeric(data[data_len - 2]);
    alph_data += ascii_to_alphanumeric(data[data_len - 1]);
    curr_bit += write_bits(codewords, curr_bit, alph_data, ALPHANUMERIC_TWO_LEN_BITS);
  }
  return curr_bit;
}

// Returns number of bits written
uint16_t encode_numeric(byte *data, uint16_t data_len, byte *codewords, uint16_t curr_bit) {
  uint32_t numeric_data;
  for (uint16_t i = 0; i < data_len - 3; i+= 3){
    numeric_data = 100*(data[i] - '0');
    numeric_data += 10*(data[i+1] - '0');
    numeric_data += data[i+2] - '0';
    curr_bit += write_bits(codewords, curr_bit, numeric_data, NUMERIC_THREE_LEN_BITS);
  }
  switch (data_len % 3) {
  case 0:
    numeric_data = 100*(data[data_len - 3] - '0');
    numeric_data += 10*(data[data_len - 2] - '0');
    numeric_data += data[data_len - 1] - '0';
    curr_bit += write_bits(codewords, curr_bit, numeric_data, NUMERIC_THREE_LEN_BITS);
  break;
  case 1:
    numeric_data = data[data_len-1] - '0';
    curr_bit += write_bits(codewords, curr_bit, numeric_data, NUMERIC_ONE_LEN_BITS);
  break;
  case 2:
    numeric_data = data[data_len-1] - '0';
    numeric_data += 10*(data[data_len-2] - '0');
    curr_bit += write_bits(codewords, curr_bit, numeric_data, NUMERIC_TWO_LEN_BITS);
  break;
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
    curr_bit += encoder(data, seg.len, extended_codewords, curr_bit);
    data += seg.len;
  }
  curr_bit += write_bits(extended_codewords, curr_bit,0, sizeof(byte));
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
