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
  EC_LOW = 0,
  EC_MID = 1,
  EC_QUARTILE = 2,
  EC_HIGH = 3,
};

struct Version {
  uint8_t version;
  enum CharCountVersion cc_version;
  uint16_t capacity_bits;
};

struct ModeSegment {
  uint16_t len;
  enum Mode mode;
};

typedef uint8_t byte;

typedef uint16_t (*ModeEncoder)(byte*, uint16_t, byte*, uint16_t);

#endif
