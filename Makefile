# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -g

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN := ${BUILD_DIR}/cc

# Find all .c files in src and map to object files in build
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: $(BIN)

# Link
$(BIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c -> .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Ensure build dir exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN)



test: all
	find tests/ -name *.c -exec make run_test TEST="{}" \;

run_test: all
	mkdir -p $(BUILD_DIR)/tests
	$(BIN) -i $(TEST) -o $(BUILD_DIR)/$(TEST).asm
# 	as $(BUILD_DIR)/$(TEST).asm -o $(BUILD_DIR)/$(TEST).o
# 	ld $(BUILD_DIR)/$(TEST).o
# 	$(BUILD_DIR)/$(TEST)