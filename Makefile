# Made with heavy help from ChatGPT

include common.mk

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
