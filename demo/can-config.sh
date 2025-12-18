#!/bin/sh
ip link set can0 type can bitrate 250783 phase-seg1 4 phase-seg2 6
ifconfig can0 up
