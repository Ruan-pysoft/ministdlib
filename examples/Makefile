CC            := gcc
# TODO: Some way to share `CFLAGS` between makefiles?
CFLAGS        := -ffreestanding -fno-builtin -nostdlib -std=c89 -pedantic -I../include/ -Wall -Wextra -Wno-builtin-declaration-mismatch
DEBUG_FLAGS   := -gdwarf -DDEBUG -O0
RELEASE_FLAGS := -O2 -DNDEBUG

BUILD_DIR := ../build

MODE ?= debug
ifeq ($(MODE),debug)
	CFLAGS += $(DEBUG_FLAGS)
	BUILD_SUBDIR := debug
else
	CFLAGS += $(RELEASE_FLAGS)
	BUILD_SUBDIR := release
endif

BIN_DIR := $(BUILD_DIR)/$(BUILD_SUBDIR)/examples/bin
LIB_DIR := $(BUILD_DIR)/$(BUILD_SUBDIR)

LDFLAGS := -L$(LIB_DIR) -lministd

EXAMPLES_SRC := $(wildcard *.c)
EXAMPLES := $(patsubst %.c,$(BIN_DIR)/%,$(EXAMPLES_SRC))

.PHONY: all
all: $(EXAMPLES)

$(BIN_DIR)/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
