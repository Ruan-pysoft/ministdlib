# Made with heavy help from ChatGPT

CC            := gcc
CFLAGS        := -ffreestanding -fno-builtin -nostdlib -std=c89 -pedantic -I./include/ -Wall -Wextra -Wno-builtin-declaration-mismatch
DEBUG_FLAGS   := -gdwarf -DDEBUG -O0
RELEASE_FLAGS := -O2 -DNDEBUG

BUILD_DIR    := build
SRC_DIR      := src
TEST_DIR     := tests
EXAMPLES_DIR := examples

MODE ?= debug
ifeq ($(MODE),debug)
	CFLAGS += $(DEBUG_FLAGS)
	BUILD_SUBDIR := debug
else
	CFLAGS += $(RELEASE_FLAGS)
	BUILD_SUBDIR := release
endif

OBJ_DIR := $(BUILD_DIR)/$(BUILD_SUBDIR)/obj

LIB := $(BUILD_DIR)/$(BUILD_SUBDIR)/libministd.a
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all
all: $(LIB)

$(LIB): $(OBJ)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: test
test: all
	$(MAKE) -C $(TEST_DIR) MODE=$(MODE) run

.PHONY: examples
examples: all
	$(MAKE) -C $(EXAMPLES_DIR) MODE=$(MODE)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
