LIB = mpsLibC
BUILD_DIR = build
TARGET = $(BUILD_DIR)/$(LIB).a

CC = gcc

C_FLAGS = \
	-Wextra \
	-Wall \
	-O3 \

LD_FLAGS = \
	-lm \
	-lpthread \

SRC_FILES = \
	RingBuffer/PFifo/PFifo.c \
	RingBuffer/LFfifo/LFfifo.c \
	ThreadPool/ThreadPool.c \

# Object files go in build/, preserving subdirectory structure
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

.PHONY: all clean

all: $(TARGET)

# Build static library
$(TARGET): $(OBJS)
	ar rcs $@ $^

# Compile object files
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -I. -c $< -o $@

# Ensure build directory exists (optional)
$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf ./$(BUILD_DIR)
