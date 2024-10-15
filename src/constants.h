#ifndef CONSTANTS
#define CONSTANTS

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

extern const uint8_t NUMERIC_CHARACTER_COUNT_LEN[3];
extern const uint8_t ALPHANUMERIC_CHARACTER_COUNT_LEN[3];
extern const uint8_t BYTE_CHARACTER_COUNT_LEN[3];

#endif
