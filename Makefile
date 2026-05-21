CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2 -I.
LDFLAGS = -lm
BUILD_DIR = build

# Определение операционной системы
ifeq ($(OS),Windows_NT)
    # Windows настройки
    LIB_NAME = libkdTree.dll
    TARGET = robot_spatial.exe
    TEST_TARGET = test.exe
    MKDIR = mkdir
    RM = rmdir /S /Q
    DLL_LINK_FLAGS = -Wl,--out-implib,$(BUILD_DIR)/libkdTree.dll.a
    EXE_EXT = .exe
    # Пути к CMocka на Windows
    CMOCKA_DIR = C:/msys64/mingw64
    CMOCKA_CFLAGS = -I$(CMOCKA_DIR)/include
    CMOCKA_LDFLAGS = -L$(CMOCKA_DIR)/lib -lcmocka
else
    # Linux настройки
    LIB_NAME = libkdTree.so
    TARGET = robot_spatial
    TEST_TARGET = test
    MKDIR = mkdir -p
    RM = rm -rf
    DLL_LINK_FLAGS =
    EXE_EXT =
    # Пути к CMocka на Linux
    CMOCKA_DIR = /usr
    CMOCKA_CFLAGS = -I$(CMOCKA_DIR)/include
    CMOCKA_LDFLAGS = -L$(CMOCKA_DIR)/lib -lcmocka
endif

LIB_SRCS = kdTree.c clustering.c
HEADERS = kdTree.h
LIB_OBJS = $(LIB_SRCS:%.c=$(BUILD_DIR)/%.o)

MAIN_SRC = main.c
MAIN_OBJ = $(BUILD_DIR)/main.o
TEST_SRC = test.c
TEST_OBJ = $(BUILD_DIR)/test.o

LIB_PATH = $(BUILD_DIR)/$(LIB_NAME)
TARGET_PATH = $(BUILD_DIR)/$(TARGET)
TEST_TARGET_PATH = $(BUILD_DIR)/$(TEST_TARGET)

# Флаги для тестов с CMocka
TEST_CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2 -I. $(CMOCKA_CFLAGS) -Wno-unused-parameter

.PHONY: all clean rebuild run test main help

all: $(TARGET_PATH) $(TEST_TARGET_PATH)
	@echo "Robot spatial built: $(BUILD_DIR)/$(TARGET)"
	@echo "Test built: $(BUILD_DIR)/$(TEST_TARGET)"

main: $(TARGET_PATH)
	@echo "Robot spatial built: $(BUILD_DIR)/$(TARGET)"

test: $(TEST_TARGET_PATH)
	@echo "Test built: $(BUILD_DIR)/$(TEST_TARGET)"

$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(BUILD_DIR)/test.o: $(TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(LIB_PATH): $(LIB_OBJS)
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(DLL_LINK_FLAGS)

$(MAIN_OBJ): $(MAIN_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_PATH): $(MAIN_OBJ) $(LIB_PATH)
	$(CC) $(MAIN_OBJ) -o $@ -L$(BUILD_DIR) -lkdTree $(LDFLAGS)

$(TEST_TARGET_PATH): $(TEST_OBJ) $(LIB_PATH)
	$(CC) $(TEST_OBJ) -o $@ -L$(BUILD_DIR) -lkdTree $(CMOCKA_LDFLAGS) $(LDFLAGS)

run: $(TARGET_PATH)
	@cd $(BUILD_DIR) && ./$(TARGET)

clean:
	$(RM) $(BUILD_DIR)

rebuild: clean all

help:
	@echo "Available targets:"
	@echo "  all   - Build robot_spatial and test"
	@echo "  main  - Build robot_spatial only"
	@echo "  test  - Build test only"
	@echo "  run   - Run robot_spatial"
	@echo "  clean - Remove build directory"
	@echo "  rebuild - Clean and rebuild"
	@echo ""
	@echo "Detected OS: $(OS)"