#include "types.h"
#include "func_table.h"
uint8_t ascii_to_alphanumeric(uint8_t c) {
  if (c >= 0x30 && c <= 0x39)
    return c - '0';
  else if (c >= 0x41 && c <= 0x5a)
    return c + 10 - 'A';
  switch (c) {
  case ':':
    return 44;
    break;
  case '/':
    return 43;
    break;
  case '.':
    return 42;
    break;
  case '-':
    return 41;
    break;
  case '+':
    return 40;
    break;
  case '*':
    return 39;
    break;
  case '%':
    return 38;
    break;
  case '$':
    return 37;
    break;
  case ' ':
    return 36;
    break;
  default:
    return 35;
    break;
  }
}

enum Mode get_mode(uint8_t c) {
  if (c >= 0x30 && c <= 0x39)
    return NUM;
  else if (c == 0x20 || c == 0x24 || c == 0x25 || c == 0x2a || c == 0x2b || (c >= 0x2d && c <= 0x2f) || c == 0x3a || (c >= 0x41 && c <= 0x5a))
    return ALPH_NUM;
  else
    return BYTE;
}

enum Mode get_worst_mode(uint8_t* data, uint16_t data_len) {
  enum Mode mode = 0;
  for (uint16_t i = 0; i < data_len; i++) {
    mode |= get_mode(data[i]); 
  }
  return mode;
}
