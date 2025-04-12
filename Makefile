FLAGS = -MMD -Iinclude
CFLAGS = -g -O0 -Wall -static
LDFLAGS = -lm

GNU = aarch64-linux-gnu

CC = $(GNU)-gcc
LD = $(GNU)-ld

BUILD_DIR = build
SRC_DIR = src

$(BUILD_DIR)/%_c.o : $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%_s.o : $(SRC_DIR)/%.s
	mkdir -p $(@D)
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<

C_SRCS = $(shell find $(SRC_DIR) -type f -name "*.c")
ASM_SRCS = $(shell find $(SRC_DIR) -type f -name "*.s")

C_OBJS = $(C_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
ASM_OBJS = $(ASM_SRCS:$(SRC_DIR)/%.s=$(BUILD_DIR)/%_s.o)

OBJS = $(C_OBJS) $(ASM_OBJS)

DEPS = $(OBJS:%.o=%.d)
-include $(DEPS)

$(BUILD_DIR)/app.elf : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

.PHONY: clean all

clean:
	rm -rf $(BUILD_DIR)

all: $(BUILD_DIR)/app.elf