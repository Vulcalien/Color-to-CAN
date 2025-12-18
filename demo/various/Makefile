# Vulcalien's Executable Makefile
# version 0.3.7

TARGET := UNIX

# ==================================================================== #
#                              Basic Info                              #
# ==================================================================== #

OUT_FILENAME := demo

SRC_DIR := .
OBJ_DIR := obj
BIN_DIR := bin

SRC_SUBDIRS :=

# ==================================================================== #
#                             Compilation                              #
# ==================================================================== #

CPPFLAGS := -MMD -MP -I../../include
CFLAGS   := -Wall -pedantic

ASFLAGS :=

ifeq ($(TARGET),UNIX)
    CC := gcc
    AS := as

    LDFLAGS :=
    LDLIBS  :=
else ifeq ($(TARGET),WINDOWS)
    CC := x86_64-w64-mingw32-gcc
    AS := x86_64-w64-mingw32-as

    LDFLAGS :=
    LDLIBS  :=
endif

# ==================================================================== #
#                        Extensions & Commands                         #
# ==================================================================== #

ifeq ($(TARGET),UNIX)
    OBJ_EXT    := o
    OUT_SUFFIX :=
else ifeq ($(TARGET),WINDOWS)
    OBJ_EXT    := obj
    OUT_SUFFIX := .exe
endif

MKDIR := mkdir -p
RM    := rm -rfv

# ==================================================================== #
#                              Resources                               #
# ==================================================================== #

SRC_EXT := c s

SRC_DIRS := $(SRC_DIR) $(foreach SUB,$(SRC_SUBDIRS),$(SRC_DIR)/$(SUB))

SRC := $(foreach DIR,$(SRC_DIRS),\
         $(foreach EXT,$(SRC_EXT),\
           $(wildcard $(DIR)/*.$(EXT))))

OBJ_DIRS := $(SRC_DIRS:%=$(OBJ_DIR)/%)

OBJ := $(SRC:%=$(OBJ_DIR)/%.$(OBJ_EXT))

OUT := $(BIN_DIR)/$(OUT_FILENAME)$(OUT_SUFFIX)

# ==================================================================== #
#                               Targets                                #
# ==================================================================== #

.PHONY: all run build clean

all: build

run:
	./$(OUT)

build: $(OUT)

clean:
	@$(RM) $(BIN_DIR) $(OBJ_DIR)

# generate output file
$(OUT): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# compile .c files
$(OBJ_DIR)/%.c.$(OBJ_EXT): %.c | $(OBJ_DIRS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# compile .s files
$(OBJ_DIR)/%.s.$(OBJ_EXT): %.s | $(OBJ_DIRS)
	$(AS) $(ASFLAGS) $< -o $@

# create directories
$(BIN_DIR) $(OBJ_DIRS):
	$(MKDIR) $@

-include $(OBJ:.$(OBJ_EXT)=.d)
