PROJECT_NAME = compiler

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BUILD_DIR = build

CC = clang
CFLAGS = -Wall -Werror -Wextra -pedantic --std=c23 -g -O3 -I$(INC_DIR)
LDFLAGS =
LDLIBS =

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
DEPS = $(OBJ:.o=.d)

TARGET = $(BUILD_DIR)/$(PROJECT_NAME).out

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

-include $(DEPS)
