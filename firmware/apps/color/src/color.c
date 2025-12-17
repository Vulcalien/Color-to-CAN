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
#include "color.h"

#include <stdio.h>
#include <unistd.h>

#include <nuttx/i2c/i2c_master.h>

#define LED_NEVER         0
#define LED_WHEN_SAMPLING 1
#define LED_ALWAYS_ON     2

extern struct i2c_master_s *i2cmain;
static struct i2c_config_s i2c_config;

static int led_usage;

extern void set_i2c_rst(bool on);

static inline void color_reset(void) {
    set_i2c_rst(0);
    usleep(2000);
    set_i2c_rst(1);
    usleep(2000);
    set_i2c_rst(0);
    usleep(10000);
}

static inline int detect_sensor(void) {
    uint8_t cmd = 0x92; // addr = 0x12 (ID register)
    uint8_t id;
    i2c_writeread(i2cmain, &i2c_config, &cmd, 1, &id, 1);
    return (id != 0x44);
}

static inline int initialize_sensor(void) {
    uint8_t buf[] = {
        0xa0, // addr = 0x00 (ENABLE register), auto-increment
        0x13, // ENABLE: Power on, RGBC enable, LED off
        0xff, // ATIME:  2.4 ms
    };
    i2c_write(i2cmain, &i2c_config, buf, sizeof(buf));
    usleep(10000); // wait 10ms
    return 0;
}

int color_init(void) {
    // prepare I2C configuration
    i2c_config.frequency = 400000; // 400 KHz
    i2c_config.address   = 0x29;
    i2c_config.addrlen   = 7;

    puts("[Color] resetting sensor");
    board_userled(BOARD_RED_LED,   true);
    board_userled(BOARD_GREEN_LED, true);
    color_reset();

    if(detect_sensor()) {
        puts("[Color] sensor not detected");
        return 1;
    }
    puts("[Color] sensor detected");

    if(initialize_sensor()) {
        puts("[Color] error initializing sensor");
        return 1;
    }
    puts("[Color] initialization complete");

    board_userled(BOARD_RED_LED,   false);
    board_userled(BOARD_GREEN_LED, false);
    return 0;
}

static inline void toggle_led(bool enable) {
    const int led_bit = (!enable) << 4;
    uint8_t buf[] = {
        0x80,           // addr = 0x00 (ENABLE register)
        0x03 | led_bit, // ENABLE: Power on, RGBC enable, LED on/off
    };
    i2c_write(i2cmain, &i2c_config, buf, sizeof(buf));
}

int color_read_data(uint16_t *r, uint16_t *g, uint16_t *b,
                    uint16_t *clear) {
    toggle_led(led_usage != LED_NEVER);
    usleep(2400);

    // read STATUS register, then read the (clear, r, g, b) values
    uint8_t cmd = 0xb3; // addr = 0x13 (STATUS register), auto-increment
    uint8_t data[9];
    i2c_writeread(i2cmain, &i2c_config, &cmd, 1, data, sizeof(data));

    toggle_led(led_usage == LED_ALWAYS_ON);

    // if data is not valid, return an error
    if(!(data[0] & 1))
        return 1;

    *clear = data[1] | data[2] << 8;
    *r     = data[3] | data[4] << 8;
    *g     = data[5] | data[6] << 8;
    *b     = data[7] | data[8] << 8;

    if(debug_flag) {
        printf(
            "[Color] read R:%d, G:%d, B:%d - C:%d\n",
            *r, *g, *b, *clear
        );
    }
    return 0;
}

int color_set_led_usage(int val) {
    int err = 0;
    switch(val) {
        case LED_NEVER:
        case LED_WHEN_SAMPLING:
        case LED_ALWAYS_ON:
            led_usage = val;
            break;

        default:
            err = 1;
    }
    printf("[Color] setting LED usage to %d (err=%d)\n", val, err);
    return err;
}
