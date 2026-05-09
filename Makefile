# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Target executable name
TARGET = hospital

# Source files
SRCS = main.c A.c B.c C.c D.c utils.c

# Object files (auto-generated from SRCS)
OBJS = $(SRCS:.c=.o)

# Default target: compile and run automatically
all: run

# Compile and link the target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run target: builds if necessary, then runs the executable
run: $(TARGET)
	./$(TARGET)

# Clean target: removes generated files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
