CC = gcc

CC_FLAGS= -Wall -Wextra -Wpedantic -Werror -c

SRC_DIR = src
BUILD_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/DEBUG/%.o)

lib: $(OBJECTS)
	$(CC) -shared -o $(BUILD_DIR)/liblogger.so $(OBJECTS)

libdebug: $(DEBUG_OBJECTS)
	$(CC) -shared -o $(BUILD_DIR)/liblogger_debug.so $(DEBUG_OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@ -fPIC

$(BUILD_DIR)/DEBUG/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)/DEBUG
	$(CC) $(CC_FLAGS) $< -o $@ -fPIC -g

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*
