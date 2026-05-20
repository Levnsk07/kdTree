# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -I. -fPIC
LDFLAGS = -shared -fPIC
LDLIBS  = -lm

# Project files
SRCS    = main.c
LIB_SRCS = kdTree.c clustering.c
OBJS    = $(SRCS:.c=.o)
LIB_OBJS = $(LIB_SRCS:.c=.o)
LIB_NAME = libkdTree.so
MAIN_NAME = $(BUILD_DIR)/robot_spatial.exe
TEST_NAME = $(BUILD_DIR)/test.exe
LIB_PATH = $(BUILD_DIR)/$(LIB_NAME)

# Directories
BUILD_DIR = build

# Phony targets
.PHONY: clean all test

# Default target
all: $(BUILD_DIR) $(LIB_PATH) $(MAIN_NAME)

# Create build directory
$(BUILD_DIR):
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# Compile dynamic library object files
$(LIB_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build dynamic library
$(LIB_PATH): $(LIB_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compile main object file
main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build main executable
$(MAIN_NAME): main.o $(LIB_PATH)
	$(CC) $(CFLAGS) -o $@ main.o $(LIB_PATH) $(LDLIBS)

# Compile test object file
Test.o: Test.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build test executable
test: $(BUILD_DIR) $(LIB_PATH) $(TEST_NAME)

$(TEST_NAME): Test.o $(LIB_PATH)
	$(CC) $(CFLAGS) -o $@ Test.o $(LIB_PATH) $(LDLIBS)

# Clean build directory
clean:
	if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	-del /Q *.o
