#include "main.h"

#include "color2can.h"

// Compile-time checks

_Static_assert(
    sizeof(struct color2can_config) == COLOR2CAN_CONFIG_SIZE,
    "size of struct color2can_config is incorrect"
);

_Static_assert(
    sizeof(struct color2can_range) == COLOR2CAN_RANGE_SIZE,
    "size of struct color2can_range is incorrect"
);

_Static_assert(
    sizeof(struct color2can_sample) == COLOR2CAN_SAMPLE_SIZE,
    "size of struct color2can_sample is incorrect"
);
