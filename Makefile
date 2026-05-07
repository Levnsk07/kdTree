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
MAIN_NAME = ./build/main
LIB_PATH = ./build/$(LIB_NAME)

# Directories
BUILD_DIR = ./build

# Phony targets
.PHONY: clean all

# Default target
all: $(BUILD_DIR) $(LIB_PATH) $(MAIN_NAME)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

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
	$(CC) $(CFLAGS) -o $@ $^ -L$(BUILD_DIR) -lkdTree $(LDLIBS)

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)