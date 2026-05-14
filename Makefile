# Hospital Management System - Makefile
# ==========================================

CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude
LDFLAGS =

SRC_DIR  = src
INC_DIR  = include
OBJ_DIR  = build
DATA_DIR = data

TARGET = hospital

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default target: compile and auto-run
all: directories $(TARGET)
	@echo ""
	@echo "  ╔═══════════════════════════════════╗"
	@echo "  ║  BUILD SUCCESSFUL                 ║"
	@echo "  ║  Starting Hospital System...      ║"
	@echo "  ╚═══════════════════════════════════╝"
	@echo ""
	./$(TARGET)

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DATA_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Explicit run target (optional, does the same as 'make')
run: directories $(TARGET)
	./$(TARGET)

# Clean everything
clean:
	@rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Cleaned build artifacts."

# Clean including data files
cleanall: clean
	@rm -f $(DATA_DIR)/users.txt $(DATA_DIR)/*.txt $(DATA_DIR)/*.json $(DATA_DIR)/*.csv
	@echo "Cleaned build artifacts and data files."

.PHONY: all run clean cleanall directories