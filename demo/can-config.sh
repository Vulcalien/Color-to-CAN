#!/bin/sh
ip link set can0 type can bitrate 250000 phase-seg1 8 phase-seg2 7
ifconfig can0 up
