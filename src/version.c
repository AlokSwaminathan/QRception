#include "constants.h"
#include "types.h"

static inline enum CharCountVersion get_cc_version(uint8_t version){
  if (version >= 1 && version <= 9)
    return CC_ONE;
  else if (version >= 10 && version <= 26)
    return CC_TWO;
  else if (version >= 27 && version <= 40)
    return CC_THREE;
}

struct Version get_smallest_version(uint16_t *sizes, enum ErrorCorrectionVersion ec) {
  enum CharCountVersion cc_version;
  uint16_t curr_size = 0;
  for (uint8_t i = 0; i < sizeof(MAX_DATA_BITS)/sizeof(MAX_DATA_BITS[0]); i++){
    cc_version = get_cc_version(i+1); 
    curr_size += MAX_DATA_BITS[i][ec];
    if (curr_size >= sizes[cc_version]){
      return (struct Version) {
        .cc_version = cc_version,
        .version = i+1,
        .capacity_bits = MAX_DATA_BITS[i][ec],
      };
    }
  }
  return (struct Version) {
    .cc_version = 255,
    .version = 255,
    .capacity_bits = 0,
  };
}