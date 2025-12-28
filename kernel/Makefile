# MetalOS Kernel Makefile

# Cross-compiler (x86_64 bare metal)
CC = x86_64-elf-gcc
AS = nasm
LD = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy

# Check if cross-compiler exists, fallback to regular gcc
ifeq ($(shell which $(CC) 2>/dev/null),)
    CC = gcc
    LD = ld
    OBJCOPY = objcopy
endif

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Compiler flags
CFLAGS = -Wall -Wextra -Werror \
         -ffreestanding -fno-stack-protector \
         -mno-red-zone -mcmodel=large \
         -I$(INC_DIR) \
         -O2

# Assembler flags
ASFLAGS = -f elf64

# Linker flags
LDFLAGS = -nostdlib -T linker.ld

# Source files
C_SOURCES = $(shell find $(SRC_DIR) -name '*.c')
ASM_SOURCES = $(shell find $(SRC_DIR) -name '*.asm')

# Object files
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Output
TARGET = metalos.bin

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/core
	mkdir -p $(BUILD_DIR)/hal
	mkdir -p $(BUILD_DIR)/drivers
	mkdir -p $(BUILD_DIR)/syscall

# Compile C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Link kernel
$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Print variables for debugging
info:
	@echo "C Sources: $(C_SOURCES)"
	@echo "ASM Sources: $(ASM_SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Compiler: $(CC)"
