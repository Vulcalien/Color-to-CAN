/* Copyright 2025 Vulcalien
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define COLOR2CAN_CONFIG_SIZE 2
struct color2can_config {
    uint8_t frequency;           // 0=only on-demand, 1...60 Hz
    uint8_t use_led         : 2; // 0=never, 1=when sampling, 2=always on
    uint8_t transmit_timing : 1; // 0=on-demand, 1=continuous
};

#define COLOR2CAN_SAMPLE_SIZE 8
struct color2can_sample {
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t clear;
};

// TODO are the numbers and IDs below good?

// number of distinct sensor IDs (ID=0 is broadcast)
#define COLOR2CAN_MAX_SENSOR_COUNT 32

#define COLOR2CAN_CONFIG_MASK_ID 0x720 // 0x720...0x73f
#define COLOR2CAN_SAMPLE_MASK_ID 0x740 // 0x740...0x75f

#ifdef __cplusplus
}
#endif
