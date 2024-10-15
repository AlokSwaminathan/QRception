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

struct ModeSegment {
  uint16_t len;
  enum Mode mode;
};

typedef uint8_t byte;

#endif
