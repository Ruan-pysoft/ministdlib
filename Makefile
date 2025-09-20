# Made with heavy help from ChatGPT

include common.mk

OBJ_DIR := $(BUILD_DIR)/$(BUILD_SUBDIR)/obj
TEST_BUILD_DIR := $(BUILD_DIR)/$(BUILD_SUBDIR)/test

LIB := $(BUILD_DIR)/$(BUILD_SUBDIR)/libministd.a
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)
TEST_OBJ := $(patsubst $(SRC_DIR)/%.c,$(TEST_BUILD_DIR)/%.o,$(SRC))
TEST_DEP := $(TEST_OBJ:.o=.d)

.PHONY: all
all: $(LIB)

$(LIB): $(OBJ)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

.PHONY: test
test: $(TEST_BUILD_DIR)/test_runner
	$(TEST_BUILD_DIR)/test_runner

$(TEST_BUILD_DIR)/test_runner: $(TEST_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DTEST $^ -o $@

$(TEST_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -DTEST -c $< -o $@

.PHONY: examples
examples: all
	$(MAKE) -C $(EXAMPLES_DIR) MODE=$(MODE)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEP)
-include $(TEST_DEP)
