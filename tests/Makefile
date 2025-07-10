include ../common.mk

CFLAGS := $(patsubst -I./%,-I../%,$(CFLAGS))

BUILD_DIR := ../build

BIN_DIR := $(BUILD_DIR)/$(BUILD_SUBDIR)/tests/bin
LIB_DIR := $(BUILD_DIR)/$(BUILD_SUBDIR)

LDFLAGS := -L$(LIB_DIR) -lministd

TESTS_SRC := $(wildcard *.c)
TESTS := $(patsubst %.c,$(BIN_DIR)/%,$(TESTS_SRC))

.PHONY: run
run: $(TESTS)

$(BIN_DIR)/%: TEST_NAME = $(patsubst $(BIN_DIR)/%,%,$@)
$(BIN_DIR)/%: %.c
	@mkdir -p $(dir $@)
	@echo -e "COMPILING \033[33m\033[1m$(TEST_NAME)\033[m..."
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo -e "RUNNING   \033[33m\033[1m$(TEST_NAME)\033[m..."
	$@ > "$(TEST_NAME).output"
	@if diff --color "$(TEST_NAME).output" "$(TEST_NAME).expected"; then\
		echo -e " === \033[32m\033[1mTEST \033[33m$(TEST_NAME)\033[32m PASSED\033[m ===";\
	else\
		echo -e " === \033[31m\033[1mTEST \033[33m$(TEST_NAME)\033[31m FAILED\033[m ===";\
	fi
