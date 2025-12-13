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
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <nuttx/can/can.h>

static int can_fd;
static int sensor_id;

/* ================================================================== */
/*                              Receiver                              */
/* ================================================================== */

#define RECEIVER_BUFFER_SIZE (sizeof(struct can_msg_s))

static inline void handle_message(const struct can_msg_s *msg) {
    // TODO
}

static void *receiver_run(void *arg) {
    printf("[CAN-IO] receiver thread started\n");

    static char buffer[RECEIVER_BUFFER_SIZE];
    while(true) {
        int offset = 0;

        // read CAN message(s)
        int nbytes = read(can_fd, buffer, RECEIVER_BUFFER_SIZE);
        if(nbytes < 0) {
            printf("[CAN-IO] error reading from CAN device\n");
            continue;
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
    return NULL;
}

/* ================================================================== */
/*                               Sender                               */
/* ================================================================== */

static void *sender_run(void *arg) {
    printf("[CAN-IO] sender thread started\n");

    while(true) {
        // TODO
    }
    return NULL;
}

/* ================================================================== */

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

int can_io_start(void) {
    // open CAN device in read-write mode
    can_fd = open("/dev/can0", O_RDWR | O_NOCTTY);
    if(can_fd < 0) {
        perror("[CAN-IO] error opening /dev/can0");
        return 1;
    }
    printf("[CAN-IO] /dev/can0 opened\n");
    print_bit_timing(can_fd);

    // create receiver thread
    pthread_t receiver_thread;
    if(pthread_create(&receiver_thread, NULL, receiver_run, NULL)) {
        printf("[CAN-IO] error creating receiver thread\n");
        return 1;
    }

    // create sender thread
    pthread_t sender_thread;
    if(pthread_create(&sender_thread, NULL, sender_run, NULL)) {
        printf("[CAN-IO] error creating sender thread\n");
        return 1;
    }
    return 0;
}

int can_io_set_sensor_id(int id) {
    int err = 0;
    if(id > 0 && id < /*TODO*/1)
        sensor_id = id;
    else
        err = 1;

    printf("[CAN-IO] setting sensor ID to %d (err=%d)\n", id, err);
    return err;
}
