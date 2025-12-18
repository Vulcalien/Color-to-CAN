#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <pthread.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>

#include "color2can.h"

static int sockfd;

#define RTR_BIT (1 << 30)

static int can_open(const char *ifname) {
    if((sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket");
        return 1;
    }

    struct ifreq ifr = { 0 };
    strcpy(ifr.ifr_name, ifname);
    ioctl(sockfd, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr = { 0 };
    addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}
    return 0;
}

static int can_write(uint32_t can_id, void *data, int len) {
    struct can_frame frame;

    frame.can_id  = can_id;
    frame.can_dlc = len;
    memcpy(frame.data, data, len);

    const int frame_size = sizeof(struct can_frame);
    if(write(sockfd, &frame, frame_size) != frame_size) {
        perror("CAN write");
        return -1;
    }
    return len;
}

static int can_read(uint32_t *can_id, void *data) {
    struct can_frame frame;

    if(read(sockfd, &frame, sizeof(struct can_frame)) < 0) {
        perror("CAN read");
        return -1;
    }

    *can_id = frame.can_id;
    int len = frame.can_dlc;
    memcpy(data, frame.data, len);
    return len;
}

static void request_sample(void) {
    printf("[Sender] requesting sample\n");

    uint8_t data[8];
    can_write(COLOR2CAN_SAMPLE_MASK_ID | RTR_BIT, data, 0);
}

static void *sender(void *arg) {
    struct color2can_config config = {
        .transmit_frequency = 0,
        .color_space = COLOR2CAN_SPACE_HSV,
        .use_led = 1
    };
    can_write(COLOR2CAN_CONFIG_MASK_ID, &config, COLOR2CAN_CONFIG_SIZE);

    struct color2can_range low_y = {
        .high = 0,
        .range_id = 0,
        .color = { 27, 200, 512 }
    };

    struct color2can_range high_y = {
        .high = 1,
        .range_id = 0,
        .color = { 87, 1024, 1024 }
    };

    struct color2can_range low_b = {
        .high = 0,
        .range_id = 1,
        .color = { 170, 200, 150 }
    };

    struct color2can_range high_b = {
        .high = 1,
        .range_id = 1,
        .color = { 230, 1024, 1024 }
    };

    can_write(COLOR2CAN_RANGE_MASK_ID, &low_y,  COLOR2CAN_RANGE_SIZE);
    can_write(COLOR2CAN_RANGE_MASK_ID, &high_y, COLOR2CAN_RANGE_SIZE);
    can_write(COLOR2CAN_RANGE_MASK_ID, &low_b,  COLOR2CAN_RANGE_SIZE);
    can_write(COLOR2CAN_RANGE_MASK_ID, &high_b, COLOR2CAN_RANGE_SIZE);

    while(1) {
        getchar();
        request_sample();
    }
    return NULL;
}

static void *receiver(void *arg) {
    while(1) {
        uint32_t can_id;
        uint8_t data[8];
        can_read(&can_id, data);

        if(can_id & RTR_BIT)
            continue;

        const int sensor_id = can_id % COLOR2CAN_MAX_SENSOR_COUNT;
        const int msg_type  = can_id - sensor_id;

        if(msg_type == COLOR2CAN_SAMPLE_MASK_ID) {
            struct color2can_sample sample;
            memcpy(&sample, data, COLOR2CAN_SAMPLE_SIZE);

            printf("[Receiver] received sample (sensor=%d):\n", sensor_id);
            printf("  color: %d, %d, %d\n", sample.color[0], sample.color[1], sample.color[2]);
            printf("  clear value: %d\n", sample.clear);
            printf("  within range: %d\n", sample.within_range ? sample.range_id : -1);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if(can_open("can0")) {
        printf("Error trying to open CAN device\n");
        return 1;
    }

    pthread_t sender_thread, receiver_thread;
    pthread_create(&sender_thread,   NULL, sender,   NULL);
    pthread_create(&receiver_thread, NULL, receiver, NULL);

    pthread_join(sender_thread,   NULL);
    pthread_join(receiver_thread, NULL);
    return 0;
}
