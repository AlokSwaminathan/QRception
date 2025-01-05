#ifndef CONSTANTS
#define CONSTANTS

#include <stdint.h>

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
#define MAX_DATA_CODEWORDS (2956 + 10)
#define MAX_CODEWORDS (3706 + 10)
#define MAX_CODEWORDS_BITS (MAX_CODEWORDS * 8)

#define QR_MATRIX_PADDING (3)
#define MAX_QR_MATRIX_SIZE (177 + 2 * (QR_MATRIX_PADDING))

#define FINDER_PATTERN_HEIGHT (9)
#define ALIGNMENT_PATTERN_HEIGHT (5)
#define MAX_UNIQUE_ALIGNMENT_COORDS (7)

#define NUMERIC_MODE_INDICATOR (0b0001)
#define ALPHANUMERIC_MODE_INDICATOR (0b0010)
#define BYTE_MODE_INDICATOR (0b0100)
#define TERMINATOR (0b0000)
#define BYTE_LEN_BITS (8)
#define ALPHANUMERIC_TWO_LEN_BITS (11)
#define ALPHANUMERIC_ONE_LEN_BITS (6)

#define NUMERIC_THREE_LEN_BITS (10)
#define NUMERIC_TWO_LEN_BITS (7)
#define NUMERIC_ONE_LEN_BITS (4)

#define NUM_VERSIONS (40)

#define MIN_VERSION_INFO_VERSION (6)

#define GALOIS_PRIME_POLYNOMIAL (0b11101)
#define MAX_ERROR_CODEWORDS (30)
#define MAX_DEGREE_GENERATOR_POLYNOMIAL (MAX_ERROR_CODEWORDS)
#define MAX_POLYNOMIAL_LEN (256)

// Calculates the QR matrix width for a 0-indexed version
#define QR_MATRIX_SIZE(version) (version * 4 + 21)

// Using these values since we want 0 to represent an uninitialized state for writing the actual data so we know which squares have been taken already
#define QR_MATRIX_WHITE_VALUE (1)
#define QR_MATRIX_BLACK_VALUE (3)
#define QR_MATRIX_DEFAULT_VALUE (0)

// This switches the value between 1 and 3
#define REVERSE_QR_MATRIX_VAL(val) (((val) + 6) % 4)

// 0 -> 1 -> white
// 1 -> 3 -> black
#define BIT_TO_QR_MATRIX_VAL(val) (((val) * 2) + 1)

// QR Code generator polynomial for (15, 5) BCH code
// Polynomial: x^10 + x^8 + x^5 + x^4 + x^2 + x + 1
#define BCH_GEN_POLY (0b10100110111)
#define BCH_LEN (15)

// QR Code generator polynomial for (18,6) Golay code
// Polynomial: x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1
#define GOLAY_GEN_POLY (0b1111100100101)
#define GOLAY_LEN (18)

// Mask for format info from BCH Code
#define FORMAT_INFO_MASK (0b101010000010010)

// These are different data masks that can be commented and uncommented
// x, y are each PADDING greater than would be expected

// True mask is the real mask algorithm like (i + j) % 2 == 0 or something like that
#define MASK_DATA(val, x, y) (~((val ^ TRUE_MASK(x, y)) * 255))
#define MASK_COORD_CORRECT(coord) ((coord) - QR_MATRIX_PADDING)

// #define DEFAULT_DATA_MASK (0b000)
// #define TRUE_MASK(x,y) ((((y) + (x)) % 2) == !(QR_MATRIX_PADDING % 2))

#define DEFAULT_DATA_MASK (0b001)
#define TRUE_MASK(x, y) (((y) % 2) == (QR_MATRIX_PADDING % 2))

// #define DEFAULT_DATA_MASK (0b010)
// #define TRUE_MASK(x,y) ((x) % 3 == (QR_MATRIX_PADDING % 3))

// #define DEFAULT_DATA_MASK (0b011)
// #define TRUE_MASK(x,y) ((MASK_COORD_CORRECT(x) + MASK_COORD_CORRECT(y)) % 3 == 0)

// #define DEFAULT_DATA_MASK (0b100)
// #define MASK_DIV(val,div) ((MASK_COORD_CORRECT(val))/(div))
// #define TRUE_MASK(x,y) ((MASK_DIV(y,2) + MASK_DIV(x,3)) % 2 == 0)

// Some general macros used for a few of these
#define MASK_MUL_MOD2(x, y) ((MASK_COORD_CORRECT(x) * MASK_COORD_CORRECT(y)) % 2)
#define MASK_MUL_MOD3(x, y) ((MASK_COORD_CORRECT(x) * MASK_COORD_CORRECT(y)) % 3)
#define MASK_ADD_MOD2(x, y) ((MASK_COORD_CORRECT(x) + MASK_COORD_CORRECT(y)) % 2)

// #define DEFAULT_DATA_MASK (0b101)
// #define TRUE_MASK(x,y) ((MASK_MUL_MOD2(x,y) + MASK_MUL_MOD3(x,y)) == 0)

// #define DEFAULT_DATA_MASK (0b110)
// #define TRUE_MASK(x,y) ((MASK_MUL_MOD2(x,y) + MASK_MUL_MOD3(x,y)) % 2 == 0)

// #define DEFAULT_DATA_MASK (0b111)
// #define TRUE_MASK(x,y) ((MASK_ADD_MOD2(x,y) + MASK_MUL_MOD3(x,y)) % 2 == 0)

// BMP Constants
#define BMP_HEADERS_LEN (14 + 40 + 8)

// 8 since 8 bits in a byte so easier to set up
#define BMP_PIXELS_PER_MODULE (8)

// 72 ppi, don't think it  matters but this is the "default"
#define BMP_PIXELS_PER_METER (2835)

#define BMP_WIDTH_HEIGHT(version_size) (((uint16_t)BMP_PIXELS_PER_MODULE) * ((2 * QR_MATRIX_PADDING) + (version_size)))
#define BMP_FILE_SIZE(width) (BMP_HEADERS_LEN + (((uint32_t)width * (uint32_t)width) / BMP_PIXELS_PER_MODULE))

// MAX * (MAX + 1) to account for padding
#define MAX_BMP_LEN (BMP_HEADERS_LEN + ((MAX_QR_MATRIX_SIZE * (MAX_QR_MATRIX_SIZE + 1)) * (BMP_PIXELS_PER_MODULE)))

#define BMP_FILE_HEADER_ONE (0x42)
#define BMP_FILE_HEADER_TWO (0x4d)

#define BMP_SECOND_HEADER_SIZE (40)
#define BMP_NUM_COLOR_PLANES (1)

#define BMP_BITS_PER_PIXEL (1)

#define BMP_WHITE_VAL (0xff)

#define BMP_FILE_HEADER_OFFSET (0)
#define BMP_FILE_SIZE_OFFSET (2)
#define BMP_DATA_OFFSET_OFFSET (10)
#define BMP_SECOND_HEADER_SIZE_OFFSET (14)
#define BMP_WIDTH_PIXELS_OFFSET (18)
#define BMP_HEIGHT_PIXELS_OFFSET (22)
#define BMP_COLOR_PLANES_OFFSET (26)
#define BMP_BITS_PER_PIXEL_OFFSET (28)
#define BMP_IMAGE_SIZE_OFFSET (34) // Apparently optional when using no compression
#define BMP_HORIZONTAL_RES_OFFSET (38)
#define BMP_VERTICAL_RES_OFFSET (42)
#define BMP_WHITE_COLOR_OFFSET (58)

// Custom constants that can replace the larger constant arrays
// Specific to QR Version 40 with EC level Low

#define EC_LEVEL (EC_LOW)

#define VERSION (39) // 0-indexed

#define VERSION_SIZE (QR_MATRIX_SIZE(VERSION))

#define DATA_CODEWORDS (2956)
#define TOTAL_CODEWORDS (3706)

#define NUMERIC_CHARACTER_COUNT_INDICATOR_LEN (14)
#define ALPHANUMERIC_CHARACTER_COUNT_INDICATOR_LEN (13)
#define BYTE_CHARACTER_COUNT_INDICATOR_LEN (16)

#define MAX_NUMERIC (7089)
#define MAX_ALPHANUMERIC (4296)
#define MAX_BYTES (2953)

#define ERR_LEN (30)

#define ERR_GROUP_ONE_LEN (19)
#define ERR_BLOCK_ONE_DATA_LEN (118)

#define ERR_GROUP_TWO_LEN (6)
#define ERR_BLOCK_TWO_DATA_LEN (119)

#define ALIGNMENT_DIFF (28)

#define FORMAT_INFO (0b111001011110011)
#define VERSION_INFO (0b101000110001101001)

extern const uint8_t RS_GEN_POLY[ERR_LEN + 1];

// String to print if data is too large
extern const char TOO_LARGE_ERROR[30];

#endif
