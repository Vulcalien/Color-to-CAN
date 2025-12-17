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
#include "processing.h"

#include <stdio.h>

#include "color2can.h"
#include "color.h"

static void (*convert_to_space)(uint16_t color[3], int r, int g, int b);

int processing_get_data(uint16_t color[3], uint16_t *clear,
                        bool *within_range, int *range_id) {
    uint16_t r, g, b, c;
    if(color_read_data(&r, &g, &b, &c))
        return 1;

    if(!convert_to_space)
        return 1;

    // convert from RGB to the configured color space
    convert_to_space(color, r, g, b);
    *clear = c;

    // check if color is within a range
    // TODO...
    *within_range = false;
    *range_id = 0;

    return 0;
}

static void rgb(uint16_t color[3], int r, int g, int b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

static void hsv(uint16_t color[3], int r, int g, int b) {
    // TODO
}

int processing_set_color_space(int color_space) {
    int err = 0;
    if(color_space == COLOR2CAN_SPACE_RGB)
        convert_to_space = rgb;
    else if(color_space == COLOR2CAN_SPACE_HSV)
        convert_to_space = hsv;
    else
        err = 1;

    printf("[Processing] set color space to %d (err=%d)\n", color_space, err);
    return err;
}
