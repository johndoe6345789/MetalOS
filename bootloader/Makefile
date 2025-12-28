# MetalOS Bootloader Makefile
# Builds UEFI bootloader (bootx64.efi)

# Cross-compiler setup
CC = gcc
LD = ld
OBJCOPY = objcopy

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Compiler flags for UEFI
CFLAGS = -Wall -Wextra -Werror \
         -ffreestanding -fno-stack-protector -fno-stack-check \
         -fshort-wchar -mno-red-zone \
         -I$(INC_DIR) \
         -DEFI_FUNCTION_WRAPPER

# Linker flags for UEFI
LDFLAGS = -shared -Bsymbolic -nostdlib \
          -znocombreloc -T uefi.lds

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Output
TARGET = bootx64.efi

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link and create EFI binary
$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(BUILD_DIR)/bootx64.so
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic \
	           -j .dynsym -j .rel -j .rela -j .reloc \
	           --target=efi-app-x86_64 \
	           $(BUILD_DIR)/bootx64.so $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Note: This is a simplified Makefile
# A real implementation would use gnu-efi or proper UEFI SDK
