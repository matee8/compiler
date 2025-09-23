PROJECT_NAME = compiler

SRC_DIR = src
BIN_DIR = bin
TEST_SRC_DIR = tests
INC_DIR = include
OBJ_DIR = obj
BUILD_DIR = build

CC = clang
CFLAGS = -Wall -Werror -Wextra -pedantic --std=c23 -g -O3 -I$(INC_DIR) -MMD -MP
LDFLAGS =
LDLIBS =

APP_SRC = $(wildcard $(BIN_DIR)/*.c)
LIB_SRC = $(wildcard $(SRC_DIR)/*.c)

APP_OBJ = $(patsubst $(BIN_DIR)/%.c, $(OBJ_DIR)/%.o, $(APP_SRC))
LIB_OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRC))

OBJ = $(APP_OBJ) $(LIB_OBJ)

TEST_SRC = $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_TARGETS = $(patsubst $(TEST_SRC_DIR)/%.c, $(BUILD_DIR)/%, $(TEST_SRC))

TARGET = $(BUILD_DIR)/$(PROJECT_NAME).out

.PHONY: all test clean

all: $(TARGET)

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

$(BUILD_DIR)/%: $(TEST_SRC_DIR)/%.c $(LIB_OBJ)
	@mkdir -p $(BUILD_DIR)
	@echo "Building and linking test: $@"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling library: $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(BIN_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling application: $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -rf $(OBJ_DIR) $(BUILD_DIR)

-include $(OBJ:.o=.d)
