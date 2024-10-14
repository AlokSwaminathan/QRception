#ifndef TYPES
#define TYPES

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
  long start;
  long end;
  enum Mode mode;
};

typedef unsigned char byte;

#define bool _Bool
#define false (0)
#define true (1)

#endif
