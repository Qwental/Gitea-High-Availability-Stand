CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 -D_POSIX_C_SOURCE=200809L -Isrc
LDFLAGS :=

SRC_DIR := src
OBJ_DIR := build/obj
BIN_DIR := build/bin
TARGET  := $(BIN_DIR)/system-info-server

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean run check-port help

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(OBJS:.o=.d)

PORT ?= 8080

run: all
	$(TARGET) $(PORT)

check-port:
	@echo "--- ss ---"
	@ss -tlnp | grep :$(PORT) || echo "not listening"
	@echo "--- iptables ---"
	@iptables -L INPUT -n 2>/dev/null | grep $(PORT) || echo "no iptables rule"

clean:
	rm -rf build/

help:
	@echo "make           - build"
	@echo "make run       - build & run (PORT=8080)"
	@echo "make check-port PORT=8080"
	@echo "make clean"

