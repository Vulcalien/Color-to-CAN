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
#include "can-io.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <nuttx/can/can.h>

#include "color2can.h"
#include "color.h"

static int can_fd;
static int sensor_id;

static int requests;

/* ================================================================== */
/*                              Receiver                              */
/* ================================================================== */

#define RECEIVER_BUFFER_SIZE (sizeof(struct can_msg_s))

static inline void handle_message(const struct can_msg_s *msg) {
    int msg_sensor_id = msg->cm_hdr.ch_id % COLOR2CAN_MAX_SENSOR_COUNT;
    int msg_type      = msg->cm_hdr.ch_id - msg_sensor_id;

    // check if message is addressed to this device (ID=0 is broadcast)
    if(msg_sensor_id != 0 && msg_sensor_id != sensor_id)
        return;

    switch(msg_type) {
        case COLOR2CAN_CONFIG_MASK_ID: {
            if(msg->cm_hdr.ch_dlc != COLOR2CAN_CONFIG_SIZE) {
                printf(
                    "[CAN-IO] malformed config message "
                    "(size=%d, expected=%d)\n",
                    msg->cm_hdr.ch_dlc, COLOR2CAN_CONFIG_SIZE
                );
                break;
            }

            // TODO ...
        } break;

        case COLOR2CAN_SAMPLE_MASK_ID: {
            // if RTR bit is set, request a data message
            if(msg->cm_hdr.ch_rtr)
                requests++;
        } break;
    }
}

static void receiver(void) {
    static char buffer[RECEIVER_BUFFER_SIZE];

    // keep reading messages until the file descriptor is empty
    while(true) {
        int offset = 0;

        // read CAN message(s)
        int nbytes = read(can_fd, buffer, RECEIVER_BUFFER_SIZE);
        if(nbytes < 0) {
            if(errno != EAGAIN)
                puts("[CAN-IO] error reading from CAN device");

            // there are no new messages: break the loop
            break;
        }

        // handle CAN message(s)
        while(offset < nbytes) {
            struct can_msg_s *msg = (struct can_msg_s *) &buffer[offset];
            handle_message(msg);

            // move buffer offset forward
            int msglen = CAN_MSGLEN(msg->cm_hdr.ch_dlc);
            offset += msglen;
        }
    }
}

/* ================================================================== */
/*                               Sender                               */
/* ================================================================== */

static inline int write_sample(struct color2can_sample *data) {
    struct can_msg_s msg;

    const int datalen = sizeof(struct color2can_sample);
    const int id = COLOR2CAN_SAMPLE_MASK_ID | sensor_id;

    // set CAN header
    msg.cm_hdr = (struct can_hdr_s) {
        .ch_id  = id,
        .ch_dlc = datalen,
        .ch_rtr = false,
        .ch_tcf = false
    };

    // set CAN data
    memcpy(msg.cm_data, data, datalen);

    // write CAN message
    const int msglen = CAN_MSGLEN(datalen);
    const int nbytes = write(can_fd, &msg, msglen);
    if(nbytes != msglen) {
        printf("[CAN-IO] error writing to CAN device\n");
        return 1;
    }
    return 0;
}

static inline int retrieve_data(struct color2can_sample *data) {
    uint16_t r, g, b, clear;
    int err = color_read_data(&r, &g, &b, &clear);

    *data = (struct color2can_sample) {
        .r = r, .g = g, .b = b, .clear = clear
    };
    return err;
}

static void sender(void) {
    // try to satisfy all pending requests
    while(requests > 0) {
        struct color2can_sample data;
        if(retrieve_data(&data))
            break;

        write_sample(&data);
        requests--;
    }
}

/* ================================================================== */

static inline int open_can_fd(void) {
    can_fd = open("/dev/can0", O_RDWR | O_NOCTTY);
    if(can_fd < 0)
        return 1;

    // enable non-blocking reads
    int flags = fcntl(can_fd, F_GETFL);
    fcntl(can_fd, F_SETFL, flags | O_NONBLOCK);

    return 0;
}

static inline void print_bit_timing(int fd) {
    struct canioc_bittiming_s bt;
    int ret = ioctl(
        fd, CANIOC_GET_BITTIMING,
        (unsigned long) ((uintptr_t) &bt)
    );

    if(ret < 0) {
        printf("[CAN-IO] bit timing not available\n");
    } else {
        printf("[CAN-IO] bit timing:\n");
        printf("   Baud: %lu\n", (unsigned long) bt.bt_baud);
        printf("  TSEG1: %u\n", bt.bt_tseg1);
        printf("  TSEG2: %u\n", bt.bt_tseg2);
        printf("    SJW: %u\n", bt.bt_sjw);
    }
}

static void *can_io_run(void *arg) {
    puts("[CAN-IO] thread started");
    while(true) {
        receiver();
        sender();
        usleep(500); // wait 0.5ms
    }
    return NULL;
}

int can_io_start(void) {
    while(open_can_fd())
        perror("[CAN-IO] error opening /dev/can0");
    puts("[CAN-IO] /dev/can0 opened");
    print_bit_timing(can_fd);

    pthread_t thread;
    if(pthread_create(&thread, NULL, can_io_run, NULL)) {
        puts("[CAN-IO] error creating thread");
        return 1;
    }
    return 0;
}

int can_io_set_sensor_id(int id) {
    int err = 0;
    if(id > 0 && id < COLOR2CAN_MAX_SENSOR_COUNT)
        sensor_id = id;
    else
        err = 1;

    printf("[CAN-IO] setting sensor ID to %d (err=%d)\n", id, err);
    return err;
}
