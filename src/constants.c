#include "constants.h"

// These all follow the versions 1-9, 10-26, 27-40
const uint8_t NUMERIC_CHARACTER_COUNT_LEN[3] = {10, 12, 14};
const uint8_t ALPHANUMERIC_CHARACTER_COUNT_LEN[3] = {9, 11, 13};
const uint8_t BYTE_CHARACTER_COUNT_LEN[3] = {8, 16, 16};

// Straight from standard
// 0 is L, 1 is M, 2 is Q, 3 is H, 4 is total codewords
// Values are additive, so the value for row 20 is the sum of all values from 0-20
const uint8_t MAX_DATA_BITS[40][5] = {
  {19, 16, 13, 9, 26},
  {15, 12, 9, 7, 18},
  {21, 16, 12, 10, 26},
  {25, 20, 14, 10, 30},
  {28, 22, 14, 10, 34},
  {28, 22, 14, 14, 38},
  {20, 16, 12, 6, 24},
  {38, 30, 22, 20, 46},
  {38, 28, 22, 14, 50},
  {42, 34, 22, 22, 54},
  {50, 38, 26, 18, 58},
  {46, 36, 26, 18, 62},
  {58, 44, 38, 22, 66},
  {33, 31, 17, 17, 49},
  {62, 50, 34, 26, 74},
  {66, 38, 30, 30, 78},
  {58, 54, 42, 30, 82},
  {74, 56, 30, 30, 86},
  {74, 64, 48, 28, 90},
  {66, 42, 40, 44, 94},
  {71, 45, 27, 21, 71},
  {74, 68, 56, 36, 102},
  {88, 78, 46, 22, 106},
  {80, 54, 50, 50, 110},
  {102, 86, 54, 24, 114},
  {94, 62, 36, 58, 118},
  {98, 66, 54, 32, 122},
  {63, 65, 63, 33, 93},
  {100, 74, 40, 40, 130},
  {104, 106, 74, 44, 134},
  {108, 82, 48, 48, 138},
  {112, 86, 82, 52, 142},
  {116, 90, 56, 56, 146},
  {120, 94, 60, 60, 150},
  {115, 87, 55, 25, 115},
  {128, 102, 68, 68, 158},
  {132, 78, 72, 42, 162},
  {136, 110, 76, 46, 166},
  {110, 114, 80, 80, 170},
  {144, 118, 84, 54, 174},
};

// For every single one of them, just store the 4 bytes for the lower value of the total codewords
// If that value evenly divides it will be only one, otherwise it will be that value with the one that is one greater than it
const uint8_t ERR_CODEWORD_BLOCKS[40][4] = {
  {26, 26, 26, 26},
  {44, 44, 44, 44},
  {70, 70, 35, 35},
  {100, 50, 50, 25},
  {134, 67, 33, 33},
  {86, 43, 43, 43},
  {98, 49, 32, 39},
  {121, 60, 40, 40},
  {146, 58, 36, 36},
  {86, 69, 43, 43},
  {101, 80, 50, 36},
  {116, 58, 46, 42},
  {133, 59, 44, 33},
  {145, 64, 36, 36},
  {109, 65, 54, 36},
  {122, 73, 43, 44},
  {135, 74, 50, 42},
  {150, 69, 50, 42},
  {141, 70, 47, 39},
  {135, 67, 54, 43},
  {144, 68, 50, 46},
  {139, 74, 54, 37},
  {151, 75, 54, 45},
  {147, 73, 54, 46},
  {132, 75, 54, 45},
  {142, 74, 50, 46},
  {152, 73, 53, 45},
  {147, 73, 54, 45},
  {146, 73, 53, 45},
  {145, 75, 54, 45},
  {145, 74, 54, 45},
  {145, 74, 54, 45},
  {145, 74, 54, 45},
  {145, 74, 54, 46},
  {151, 75, 54, 45},
  {151, 75, 54, 45},
  {152, 74, 54, 45},
  {152, 74, 54, 45},
  {147, 75, 54, 45},
  {148, 75, 54, 45},
};
