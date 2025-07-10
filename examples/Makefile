include ../common.mk

CFLAGS := $(patsubst -I./%,-I../%,$(CFLAGS))

BUILD_DIR := ../build

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
