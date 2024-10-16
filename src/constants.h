#ifndef CONSTANTS
#define CONSTANTS

#include "types.h"

// For simplicity this is calculated assuming max amount of bits for character count and mode
// indicator
//
// But then I used equality as the minimum so for the max numbers this would be space
// equivalent, but for smaller versions it would be less space
//
// Mode indicator - 4 bits Numeric mode count - 14 bits
//
// Alphanumeric mode count - 13 bits
#define BYTES_TO_NUMERIC_MIN (4)
#define BYTES_TO_ALPHANUMERIC_MIN (8)
#define ALPHANUMERIC_TO_NUMERIC_MIN (9)

// Minimum amount of one type before another such that it becomes worth it to keep the previous type
// as it was
#define NUMERIC_BEFORE_BYTES_MIN (4)
#define ALPHANUMERIC_BEFORE_BYTES_MIN (7)
#define NUMERIC_BEFORE_ALPHANUMERIC_MIN (9)

#define MODE_INDICATOR_LEN_BITS (4)
#define DISTINCT_CHARACTER_COUNT_SIZES (3)

// 1000 should be more than enough and not a memory issue on modern systems
// Heap allocation is too much code size overhead
#define MAX_MODE_SEGMENTS (1000)

// From QR Spec (2956) with some extra bytes since the bit insertion might write over
#define MAX_CODEWORDS (2956+10)

#define NUMERIC_MODE_INDICATOR (0b0001)
#define ALPHANUMERIC_MODE_INDICATOR (0b0010)
#define BYTE_MODE_INDICATOR (0b0100)

#define BYTE_LEN_BITS (8)

#define ALPHANUMERIC_TWO_LEN_BITS (11)
#define ALPHANUMERIC_ONE_LEN_BITS (6)

#define NUMERIC_THREE_LEN_BITS (10)
#define NUMERIC_TWO_LEN_BITS (7)
#define NUMERIC_ONE_LEN_BITS (4)

extern const uint8_t NUMERIC_CHARACTER_COUNT_LEN[3];
extern const uint8_t ALPHANUMERIC_CHARACTER_COUNT_LEN[3];
extern const uint8_t BYTE_CHARACTER_COUNT_LEN[3];

extern const uint8_t MAX_DATA_BITS[40][5];
extern const uint8_t ERR_CODEWORD_BLOCKS[40][4];

#define GALOIS_PRIME_POLYNOMIAL (0b11101)
#define MAX_ERROR_CODEWORDS (30)

#endif
