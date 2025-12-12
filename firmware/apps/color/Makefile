include $(APPDIR)/Make.defs

PROGNAME = color
PRIORITY = SCHED_PRIORITY_MAX
STACKSIZE = 2048
MODULE = "color"

CSRCS   = $(wildcard src/*.c)
MAINSRC = src/main.c

CFLAGS += -Iinclude -I../../include

include $(APPDIR)/Application.mk
