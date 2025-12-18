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

#define RANGES_COUNT 16
static struct {
    bool low_set;
    int low[3];

    bool high_set;
    int high[3];
} ranges[RANGES_COUNT];

static void (*convert_to_space)(int color[3], int r, int g, int b);

static bool is_color_in_range(int color[3], int range) {
    int *low  = ranges[range].low;
    int *high = ranges[range].high;

    return (
        low[0] <= color[0] && color[0] <= high[0] &&
        low[1] <= color[1] && color[1] <= high[1] &&
        low[2] <= color[2] && color[2] <= high[2]
    );
}

int processing_get_data(int color[3], int *clear,
                        bool *within_range, int *range_id) {
    int r, g, b, c;
    if(color_read_data(&r, &g, &b, &c))
        return 1;

    if(!convert_to_space)
        return 1;

    // convert from RGB to the configured color space
    convert_to_space(color, r, g, b);
    *clear = c;

    // check if color is within a range
    *within_range = false;
    for(int i = 0; i < RANGES_COUNT; i++) {
        if(!ranges[i].low_set || !ranges[i].high_set)
            continue;

        if(is_color_in_range(color, i)) {
            if(debug_flag)
                printf("[Processing] color is in range %d\n", i);
            *within_range = true;
            *range_id = i;
            break;
        }
    }

    return 0;
}

static void rgb(int color[3], int r, int g, int b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

static void hsv(int color[3], int r, int g, int b) {
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

    // invalidate all ranges
    for(int i = 0; i < RANGES_COUNT; i++) {
        ranges[i].low_set  = false;
        ranges[i].high_set = false;
    }

    printf("[Processing] set color space to %d (err=%d)\n", color_space, err);
    return err;
}

int processing_set_range(int id, bool high, int color[3]) {
    int *dest;
    if(high) {
        dest = ranges[id].high;
        ranges[id].high_set = true;
    } else {
        dest = ranges[id].low;
        ranges[id].low_set = true;
    }

    printf(
        "[Processing] setting range %d %s (",
        id, high ? "high" : "low "
    );
    for(int i = 0; i < 3; i++) {
        dest[i] = color[i];
        printf("%d%s", dest[i], i != 2 ? ", " : ")");
    }
    puts("");
    return 0;
}
