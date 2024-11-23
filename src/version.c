#include "constants.h"
#include "types.h"

static inline enum CharCountVersion get_cc_version(const uint8_t version) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
  if (version >= 1 && version <= 9)
    return CC_ONE;
  else if (version >= 10 && version <= 26)
    return CC_TWO;
  else if (version >= 27 && version <= 40)
    return CC_THREE;
}
#pragma GCC diagnostic pop

struct Version get_smallest_version(const uint16_t sizes[3], const enum ErrorCorrectionVersion ec) {
  enum CharCountVersion cc_version;
  uint16_t curr_size = 0;
  uint16_t cw_capacity = 0;
  for (uint8_t i = 0; i < sizeof(MAX_DATA_BITS) / sizeof(MAX_DATA_BITS[0]); i++) {
    cc_version = get_cc_version(i + 1);
    curr_size += MAX_DATA_BITS[i][ec];
    cw_capacity += MAX_DATA_BITS[i][4];
    if (curr_size >= sizes[cc_version]) {
      return (struct Version){
          .cc_version = cc_version,
          .version = i,
          .ec_version = ec,
          .capacity = curr_size,
          .cw_capacity = cw_capacity,
      };
    }
  }
  return (struct Version){
      .cc_version = 255,
      .version = 255,
      .ec_version = 255,
      .capacity = 0,
      .cw_capacity = 0,
  };
}
