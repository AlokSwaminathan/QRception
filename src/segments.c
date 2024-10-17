#include "types.h"
#include "func_table.h"
#include "constants.h"

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

// Expects sizes to be uint16_t[3] with the sizes depending on QR version
// 0 - Versions 1-9
// 1 - Versions 10-26
// 2 - Versions 27-40
// Writes in bytes
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
    sizes[i] /= 8;
  }
  segments[switches] = (struct ModeSegment){
    .mode = curr_mode,
    .len = match_streak
  };
  switches++;
  return switches;
}
