# Building MetalOS

## Prerequisites

### Required Tools

1. **GNU Compiler Collection (GCC)**
   - Cross-compiler for x86_64-elf target
   - Version 11.0 or later recommended

2. **GNU Binutils**
   - Cross-binutils for x86_64-elf target
   - Includes ld, as, objcopy, etc.

3. **NASM**
   - Netwide Assembler for x86_64
   - Version 2.15 or later

4. **GNU Make**
   - Build automation
   - Version 4.0 or later

5. **QEMU** (for testing)
   - QEMU system x86_64
   - Version 6.0 or later with UEFI support

6. **EDK II OVMF**
   - UEFI firmware for QEMU
   - Required for UEFI boot testing

### Installing Prerequisites on Ubuntu/Debian

```bash
# Install basic build tools
sudo apt-get update
sudo apt-get install -y build-essential nasm qemu-system-x86 ovmf mtools xorriso

# Install cross-compiler prerequisites
sudo apt-get install -y libgmp-dev libmpfr-dev libmpc-dev texinfo

# Note: You may need to build the cross-compiler manually
# See docs/CROSS_COMPILER.md for instructions
```

### Installing Prerequisites on Arch Linux

```bash
sudo pacman -S base-devel nasm qemu-full edk2-ovmf mtools xorriso
```

### Installing Prerequisites on macOS

```bash
brew install nasm qemu x86_64-elf-gcc x86_64-elf-binutils
```

## Building the Bootloader

```bash
# From repository root
cd bootloader
make
```

This produces `bootloader/bootx64.efi` - the UEFI bootloader.

## Building the Kernel

```bash
# From repository root
cd kernel
make
```

This produces `kernel/metalos.bin` - the kernel binary.

## Creating Bootable Image

```bash
# From repository root
make image
```

This creates `build/metalos.img` - a bootable disk image containing:
- EFI System Partition with bootloader
- Kernel binary
- Any required data files

## Testing in QEMU

MetalOS includes comprehensive QEMU support with UEFI firmware (OVMF) for testing.

### Prerequisites

Ensure QEMU and OVMF are installed:

```bash
# Ubuntu/Debian
sudo apt-get install qemu-system-x86 ovmf mtools

# Arch Linux
sudo pacman -S qemu-full edk2-ovmf mtools

# Fedora
sudo dnf install qemu-system-x86 edk2-ovmf mtools
```

### Boot MetalOS in QEMU

```bash
make qemu
```

This will:
1. Build bootloader and kernel (or use placeholders if build fails)
2. Create a bootable FAT32 disk image with UEFI boot structure
3. Launch QEMU with OVMF UEFI firmware in headless mode
4. Boot the system (will drop to UEFI shell until bootloader is complete)

**Display Options**: By default, QEMU runs in headless mode (no graphics). To use graphical display:

```bash
# Use GTK display (if available)
make qemu QEMU_DISPLAY=gtk

# Use SDL display (if available)
make qemu QEMU_DISPLAY=sdl
```

### Boot with Debug Output

```bash
make qemu-debug
```

This includes CPU interrupt and reset debugging output.

### Boot with GDB Support

For debugging with GDB:

```bash
# Terminal 1 - Start QEMU with GDB server
make qemu-gdb

# Terminal 2 - Connect GDB
gdb kernel/metalos.bin
(gdb) target remote localhost:1234
(gdb) continue
```

QEMU will wait for GDB connection before starting execution.

### Test QEMU UEFI Setup

To verify QEMU and OVMF are properly installed without needing a bootable OS image:

```bash
make qemu-uefi-test
```

This boots directly to the UEFI shell, confirming your QEMU+OVMF setup works correctly.

## Testing on Real Hardware

⚠️ **WARNING**: Testing on real hardware can be risky. Always backup your data.

### USB Boot Drive Creation

```bash
# Create bootable USB (replace /dev/sdX with your USB device)
sudo dd if=build/metalos.img of=/dev/sdX bs=4M status=progress
sync
```

### Requirements for Hardware Testing
- AMD64 system with UEFI firmware
- Radeon RX 6600 GPU (for full functionality)
- USB drive (1GB or larger)
- Ability to boot from USB in UEFI mode

## Build Configuration

Build options can be configured in `config.mk`:

```makefile
# Debug build (includes symbols, verbose output)
DEBUG ?= 1

# Optimization level (0, 1, 2, 3, s)
OPT_LEVEL ?= 2

# Target architecture
ARCH ?= x86_64

# Enable specific features
ENABLE_LOGGING ?= 1
ENABLE_SERIAL ?= 1
```

## Clean Builds

```bash
# Clean all build artifacts
make clean

# Clean everything including dependencies
make distclean
```

## Troubleshooting

### Cross-Compiler Not Found
If you get errors about missing cross-compiler:
1. Check that x86_64-elf-gcc is in your PATH
2. See `docs/CROSS_COMPILER.md` for building instructions

### QEMU UEFI Firmware Missing
```bash
# Ubuntu/Debian
sudo apt-get install ovmf

# Arch
sudo pacman -S edk2-ovmf
```

### Build Fails with "Permission Denied"
```bash
# Make sure scripts are executable
chmod +x scripts/*.sh
```

## Next Steps

After successful build:
1. Review `docs/ARCHITECTURE.md` for system design
2. See `docs/DEVELOPMENT.md` for development workflow
3. Check `docs/DEBUGGING.md` for debugging techniques
