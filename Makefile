PROJECT_NAME = compiler

SRC_DIR = src
BIN_DIR = bin
TEST_DIR = tests
INC_DIR = include
OBJ_DIR = obj
BUILD_DIR = build

CC = clang
CFLAGS = -Wall -Werror -Wextra -pedantic --std=c23 -g -I$(INC_DIR) -MMD -MP
LDFLAGS =
LDLIBS =

VPATH = $(SRC_DIR):$(BIN_DIR)

APP_SRC := $(wildcard $(BIN_DIR)/*.c)
LIB_SRC := $(shell find $(SRC_DIR) -type f -name '*.c')
TEST_SRC := $(shell find $(TEST_DIR) -type f -name '*.c')
ALL_SRC := $(APP_SRC) $(LIB_SRC)

APP_OBJ := $(patsubst $(BIN_DIR)/%.c, $(OBJ_DIR)/%.o, $(APP_SRC))
LIB_OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRC))
OBJ := $(APP_OBJ) $(LIB_OBJ)

TARGET = $(BUILD_DIR)/$(PROJECT_NAME)

TEST_TARGETS := $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%, $(TEST_SRC))

CHECK_FILES := $(shell find $(BIN_DIR) $(SRC_DIR) $(TEST_DIR) -type f -name '*.c') $(shell find $(INC_DIR) -type f -name '*.h')
TIDY_FILES := $(ALL_SRC) $(TEST_SRC)

.PHONY: all test clean check format-check tidy-check format tidy-fix

all: $(TARGET)

check: format-check tidy-check

format-check:
	@echo "--- Checking formatting with clang-format ---"
	@clang-format --style=file --dry-run -Werror $(CHECK_FILES)
	@echo "Formatting is correct."

tidy-check:
	@echo "--- Checking for lints with clang-tidy ---"
	@clang-tidy $(TIDY_FILES) -- $(CFLAGS)
	@echo "Clang-tidy found no issues."

format:
	@echo "--- Applying clang-format style ---"
	@clang-format -i $(CHECK_FILES)

tidy-fix:
	@echo "--- Applying clang-tidy fixes ---"
	@clang-tidy $(TIDY_CHECKS) -fix $(TIDY_FILES) -- $(CFLAGS)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	@echo "Linking main application: $@"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test: $(TEST_TARGETS)
	@echo "--- Running all tests ---"
	@for test_exec in $(TEST_TARGETS); do \
		./$$test_exec; \
	done
	@echo "--- All tests completed successfully ---"

$(TEST_TARGETS): $(BUILD_DIR)/%: $(TEST_DIR)/%.c $(LIB_OBJ)
	@mkdir -p $(dir $@)
	@echo "Building and linking test: $@"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -rf $(OBJ_DIR) $(BUILD_DIR)

-include $(OBJ:.o=.d)
