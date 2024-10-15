#include "types.h"
#include "func_table.h"

byte ascii_to_alphanumeric(byte c) {
  if (c >= 0x30 && c <= 0x39)
    return c - '0';
  else if (c >= 0x41 && c <= 0x5a)
    return c + 10 - 'A';
  byte ret = 35;
  switch (c) {
  case ':':
    ret++;
  case '/':
    ret++;
  case '.':
    ret++;
  case '-':
    ret++;
  case '+':
    ret++;
  case '*':
    ret++;
  case '%':
    ret++;
  case '$':
    ret++;
  case ' ':
    ret++;
    break;
  }
  return ret;
}

enum Mode get_mode(byte c) {
  if (c >= 0x30 && c <= 0x39)
    return NUM;
  else if (c == 0x20 || c == 0x24 || c == 0x25 || c == 0x2a || c == 0x2b || (c >= 0x2d && c <= 0x2f) || c == 0x3a || (c >= 0x41 && c <= 0x5a))
    return ALPH_NUM;
  else
    return BYTE;
}
