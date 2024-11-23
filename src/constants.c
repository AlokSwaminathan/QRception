#include "constants.h"

// These all follow the versions 1-9, 10-26, 27-40
const uint8_t NUMERIC_CHARACTER_COUNT_LEN[3] = {10, 12, 14};
const uint8_t ALPHANUMERIC_CHARACTER_COUNT_LEN[3] = {9, 11, 13};
const uint8_t BYTE_CHARACTER_COUNT_LEN[3] = {8, 16, 16};

// Straight from standard
// 0 is M
// 1 is L
// 2 is H
// 3 is Q
// This corresponds with their 2 bit representations in format info and how they are shown in the enum
// So that way you can index the subarray with the enum value
// Values are additive, so the value for row 20 is the sum of all values from 0-20
const uint8_t MAX_DATA_BITS[40][5] = {
  {16, 19, 9, 13, 26},
  {12, 15, 7, 9, 18},
  {16, 21, 10, 12, 26},
  {20, 25, 10, 14, 30},
  {22, 28, 10, 14, 34},
  {22, 28, 14, 14, 38},
  {16, 20, 6, 12, 24},
  {30, 38, 20, 22, 46},
  {28, 38, 14, 22, 50},
  {34, 42, 22, 22, 54},
  {38, 50, 18, 26, 58},
  {36, 46, 18, 26, 62},
  {44, 58, 22, 38, 66},
  {31, 33, 17, 17, 49},
  {50, 62, 26, 34, 74},
  {38, 66, 30, 30, 78},
  {54, 58, 30, 42, 82},
  {56, 74, 30, 30, 86},
  {64, 74, 28, 48, 90},
  {42, 66, 44, 40, 94},
  {45, 71, 21, 27, 71},
  {68, 74, 36, 56, 102},
  {78, 88, 22, 46, 106},
  {54, 80, 50, 50, 110},
  {86, 102, 24, 54, 114},
  {62, 94, 58, 36, 118},
  {66, 98, 32, 54, 122},
  {65, 63, 33, 63, 93},
  {74, 100, 40, 40, 130},
  {106, 104, 44, 74, 134},
  {82, 108, 48, 48, 138},
  {86, 112, 52, 82, 142},
  {90, 116, 56, 56, 146},
  {94, 120, 60, 60, 150},
  {87, 115, 25, 55, 115},
  {102, 128, 68, 68, 158},
  {78, 132, 42, 72, 162},
  {110, 136, 46, 76, 166},
  {114, 110, 80, 80, 170},
  {118, 144, 54, 84, 174},
};

// For every single one of them, just store the 4 bytes for the lower value of the total codewords
// 0 is M
// 1 is L
// 2 is H
// 3 is Q
// This corresponds with their 2 bit representations in format info and how they are shown in the enum
// If this value evenly divides the total number of codewords then there is only one block, otherwise we need to find another block, but regardless we can extrapolate all the info form this one number per EC level and version pair
const uint8_t ERR_CODEWORD_BLOCKS[40][4] = {
  {26, 26, 26, 26},
  {44, 44, 44, 44},
  {70, 70, 35, 35},
  {50, 100, 25, 50},
  {67, 134, 33, 33},
  {43, 86, 43, 43},
  {49, 98, 39, 32},
  {60, 121, 40, 40},
  {58, 146, 36, 36},
  {69, 86, 43, 43},
  {80, 101, 36, 50},
  {58, 116, 42, 46},
  {59, 133, 33, 44},
  {64, 145, 36, 36},
  {65, 109, 36, 54},
  {73, 122, 45, 43},
  {74, 135, 42, 50},
  {69, 150, 42, 50},
  {70, 141, 39, 47},
  {67, 135, 43, 54},
  {68, 144, 46, 50},
  {74, 139, 37, 54},
  {75, 151, 45, 54},
  {73, 147, 46, 54},
  {75, 132, 45, 54},
  {74, 142, 46, 50},
  {73, 152, 45, 53},
  {73, 147, 45, 54},
  {73, 146, 45, 53},
  {75, 145, 45, 54},
  {74, 145, 45, 54},
  {74, 145, 45, 54},
  {74, 145, 45, 54},
  {74, 145, 46, 54},
  {75, 151, 45, 54},
  {75, 151, 45, 54},
  {74, 152, 45, 54},
  {74, 152, 45, 54},
  {75, 147, 45, 54},
  {75, 148, 45, 54},
};

// The constant differences between the coordinate possibilites for alignment pairs
// There are always n constant differences and 1 other/same difference for n+2 total coordinate possibilites
const uint8_t ALIGNMENT_PATTERN_DIFFS[NUM_VERSIONS] = {
  // First couple are useless since only 2 alignment coords and we know the smaller one is 6, but probably more space to write code to accomodate that and shorten this array
  0,
  12,
  16,
  20,
  24,
  28,
  16,
  18,
  20,
  22,
  24,
  26,
  28,
  20,
  22,
  24,
  24,
  26,
  28,
  28,
  22,
  24,
  24,
  26,
  26,
  28,
  28,
  24,
  24,
  26,
  26,
  26,
  28,
  28,
  24,
  26,
  26,
  26,
  28,
  28
};
