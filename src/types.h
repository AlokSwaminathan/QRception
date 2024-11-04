#ifndef TYPES
#define TYPES

#include <stdint.h>
#include <stdbool.h>

enum Mode {
  NUM = 0,
  ALPH_NUM = 1,
  BYTE = 2,
};

enum SwitchMode {
  NO_SWITCH = 0,
  PROMOTE = 1,
  DEMOTE = 2,
};

enum CharCountVersion {
  CC_ONE = 0,
  CC_TWO = 1,
  CC_THREE = 2,
};

enum ErrorCorrectionVersion {
  EC_LOW = 0b01,
  EC_MID = 0b00,
  EC_QUARTILE = 0b11,
  EC_HIGH = 0b10,
};

struct Version {
  uint8_t version;
  enum CharCountVersion cc_version;
  enum ErrorCorrectionVersion ec_version;
  uint16_t capacity;
  uint16_t cw_capacity;
};

struct ModeSegment {
  uint16_t len;
  enum Mode mode;
};

struct ErrData {
  uint8_t block_lens[2];
  uint8_t data_lens[2];
  uint8_t err_len;
};

typedef uint16_t (*ModeEncoder)(uint8_t*, uint16_t, uint8_t*, uint16_t);

#endif
