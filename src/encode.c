#include "constants.h"
#include "func_table.h"
#include "types.h"

// Expects sizes to be a long[3] with the sizes depending on QR version
// 0 - Versions 1-9
// 1 - Versions 10-26
// 2 - Versions 27-40
void get_mode_specific_size(long *sizes, enum Mode mode, long len) {
  const int *character_counts;
  long base_size = MODE_INDICATOR_LEN_BITS;
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
  for (int i = 0; i < DISTINCT_CHARACTER_COUNT_SIZES; i++) {
    sizes[i] = base_size + character_counts[i];
  }
}

enum SwitchMode should_switch(enum Mode curr_mode, enum Mode new_mode, int match_streak,
                              int miss_streak) {
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

enum Mode get_mode(unsigned char c) {
  if (c >= 0x30 && c <= 0x39)
    return NUM;
  else if (c == 0x20 || c == 0x24 || c == 0x25 || c == 0x2a || c == 0x2b ||
           (c >= 0x2d && c <= 0x2f) || c == 0x3a || (c >= 0x41 && c <= 0x5a))
    return ALPH_NUM;
  else
    return BYTE;
}

// Expects sizes to be long[3] with the sizes depending on QR version
// 0 - Versions 1-9
// 1 - Versions 10-26
// 2 - Versions 27-40
void calculate_total_size(long *sizes, byte *msg, long len) {
  sizes[0] = 0;
  sizes[1] = 0;
  sizes[2] = 0;
  long old_sizes[3];
  byte *curr = msg;
  enum Mode curr_mode = get_mode(*curr);
  int match_streak = 1;
  int miss_streak = 0;
  for (int i = 1; i < len; i++) {
    enum Mode mode = get_mode(msg[i]);
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
  for (int i = 0; i < DISTINCT_CHARACTER_COUNT_SIZES; i++) {
    sizes[i] += old_sizes[i];
  }
}
