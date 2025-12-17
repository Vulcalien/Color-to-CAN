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
    int max = r;
    if(g > max) max = g;
    if(b > max) max = b;

    int min = r;
    if(g < min) min = g;
    if(b < min) min = b;

    int chroma = max - min;

    // calculate hue
    int hue = 0;
    if(chroma != 0) {
        if(max == r)
            hue = (60 * (g - b) / chroma + 360) % 360;
        else if(max == g)
            hue = 60 * (b - r) / chroma + 120;
        else
            hue = 60 * (r - g) / chroma + 240;
    }

    // calculate saturation
    int saturation = 0;
    if(max != 0)
        saturation = 1023 * chroma / max;

    // calculate value
    int value = max;

    color[0] = hue;
    color[1] = saturation;
    color[2] = value;
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
