CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2 -fPIC -I.
LDFLAGS = -lm
BUILD_DIR = build
LIB_NAME = libkdTree.so
TARGET = robot_spatial
LIB_SRCS = kdTree.c clustering.c
HEADERS = kdTree.h
LIB_OBJS = \
	$(BUILD_DIR)/kdTree.o \
	$(BUILD_DIR)/clustering.o
MAIN_SRC = main.c
MAIN_OBJ = $(BUILD_DIR)/main.o
LIB_PATH = $(BUILD_DIR)/$(LIB_NAME)
TARGET_PATH = $(BUILD_DIR)/$(TARGET)
# ===================================================
all: $(TARGET_PATH)

$(BUILD_DIR): # TODO How on Windows?
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/kdTree.o: kdTree.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/clustering.o: clustering.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_PATH): $(LIB_OBJS)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

$(MAIN_OBJ): $(MAIN_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_PATH): $(MAIN_OBJ) $(LIB_PATH)
	$(CC) $(MAIN_OBJ) -o $@ \
		-L$(BUILD_DIR) -lkdTree \
		-Wl,-rpath,'$$ORIGIN' \
		$(LDFLAGS)

run: $(TARGET_PATH)
	./$(TARGET_PATH)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

