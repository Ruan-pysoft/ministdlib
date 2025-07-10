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
