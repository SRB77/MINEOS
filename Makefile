# MineOS — Makefile
# Build system for the MineOS virtual operating system.
#
# Usage:
#   make        — compile all sources, produce ./mine binary
#   make run    — compile and run
#   make clean  — remove all build artifacts

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Source files
SRC = main.c \
      libs/math.c libs/string.c libs/memory.c libs/screen.c libs/keyboard.c \
      core/fs.c core/shell.c core/commands.c core/scheduler.c

# Object files (derived from source files)
OBJ = $(SRC:.c=.o)

# Output binary
TARGET = mine

# Default target
all: $(TARGET)

# Link all object files into the final binary
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile .c → .o (implicit rule uses CC and CFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET) mineos.img

# Build and run
run: all
	./$(TARGET)

.PHONY: all clean run
