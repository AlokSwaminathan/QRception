#ifndef TYPES
#define TYPES

#include "constants.h"
#include <stdint.h>
#include <stdbool.h>

enum Mode {
  NUM = 0,
  ALPH_NUM = 1,
  // Set up like this so oring will lead to BYTE if BYTE ever shows up, which is what is intended
  BYTE = 3,
};

enum CharCountVersion {
  CC_ONE = 0,
  CC_TWO = 1,
  CC_THREE = 2,
};

// These numbers correspond to their values in the format information
enum ErrorCorrectionVersion {
  EC_LOW = 0b01,
  EC_MID = 0b00,
  EC_QUARTILE = 0b11,
  EC_HIGH = 0b10,
};

struct Version {
  // 0-indexed, this principle is held up everywhere
  const uint8_t version;
  const enum CharCountVersion cc_version;
  const enum ErrorCorrectionVersion ec_version;
  const uint16_t capacity;
  const uint16_t cw_capacity;
};

struct ModeSegment {
  const uint16_t len;
  const enum Mode mode;
};

struct ErrData {
  const uint8_t block_lens[2];
  const uint8_t data_lens[2];
  const uint8_t err_len;
};

typedef uint16_t (*ModeEncoder)(const uint8_t[MAX_DATA_CODEWORDS], const uint16_t, uint8_t[MAX_DATA_CODEWORDS], uint16_t);

#endif
