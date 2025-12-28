# Building MetalOS

MetalOS can be built either using Docker (recommended for consistent builds) or natively on your system.

## Docker Build (Recommended)

The easiest way to build MetalOS is using Docker, which provides a pre-configured environment with all dependencies.

### Prerequisites

- **Docker**: Docker Engine 20.10 or later
  - [Install Docker](https://docs.docker.com/get-docker/)

### Quick Start

```bash
# 1. Build the Docker image
./scripts/docker-build.sh

# 2. Setup dependencies (downloads AMD GPU firmware, etc.)
./scripts/docker-run.sh scripts/setup-deps.sh

# 3. Build MetalOS
mkdir build && cd build
cmake ..
cmake --build .

# 4. Test in QEMU (headless mode)
cmake --build . --target qemu

# 5. Optional: Interactive shell in container
./scripts/docker-run.sh /bin/bash
```

### What's Included in Docker

The Docker image includes:
- **Build tools**: GCC, NASM, CMake, Meson
- **QEMU**: For testing with UEFI firmware
- **OVMF**: UEFI firmware for QEMU
- **Dependency management**: Scripts to download AMD firmware, Mesa RADV, QT6

### Docker Build Benefits

- ✅ Consistent build environment across all platforms
- ✅ No need to install cross-compiler manually
- ✅ Pre-configured QEMU and OVMF setup
- ✅ Isolated from host system
- ✅ Easy CI/CD integration

## Native Build

If you prefer to build natively without Docker:

### Prerequisites

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
# From repository root, if you haven't already
mkdir build && cd build
cmake ..

# Create the bootable image
cmake --build . --target image
```

This creates `build/build/metalos.img` - a bootable disk image containing:
- EFI System Partition with bootloader
- Kernel binary
- Any required data files

## Testing in QEMU

MetalOS includes comprehensive QEMU support with UEFI firmware (OVMF) for testing.

### Prerequisites

Ensure QEMU and OVMF are installed:

```bash
# Ubuntu/Debian
sudo apt-get install qemu-system-x86 ovmf mtools cmake

# Arch Linux
sudo pacman -S qemu-full edk2-ovmf mtools cmake

# Fedora
sudo dnf install qemu-system-x86 edk2-ovmf mtools cmake
```

### Boot MetalOS in QEMU

```bash
cd build
cmake --build . --target qemu
```

This will:
1. Build bootloader and kernel
2. Create a bootable FAT32 disk image with UEFI boot structure
3. Launch QEMU with OVMF UEFI firmware in headless mode
4. Boot the system

### Boot with Debug Output

```bash
cd build
cmake --build . --target qemu-debug
```

This includes CPU interrupt and reset debugging output.

### Boot with GDB Support

For debugging with GDB:

```bash
# Terminal 1 - Start QEMU with GDB server
cd build
cmake --build . --target qemu-gdb

# Terminal 2 - Connect GDB
gdb kernel/metalos.bin
(gdb) target remote localhost:1234
(gdb) continue
```

QEMU will wait for GDB connection before starting execution.

### Test QEMU UEFI Setup

To verify QEMU and OVMF are properly installed without needing a bootable OS image:

```bash
cd build
cmake --build . --target qemu-uefi-test
```

This boots directly to the UEFI shell, confirming your QEMU+OVMF setup works correctly.

## Managing Dependencies

MetalOS requires several third-party dependencies for GPU support and application framework.

### Dependency Setup Script

Use the provided script to download and setup dependencies:

```bash
# Setup all dependencies
./scripts/setup-deps.sh all

# Or setup individually
./scripts/setup-deps.sh firmware    # AMD GPU firmware blobs
./scripts/setup-deps.sh ovmf        # UEFI firmware
./scripts/setup-deps.sh mesa        # Mesa RADV (planned)
./scripts/setup-deps.sh qt6         # QT6 framework (planned)
```

### Required Dependencies

1. **AMD GPU Firmware** (`deps/firmware/`)
   - Radeon RX 6600 (Navi 23) firmware files
   - Automatically downloaded from linux-firmware repository
   - Files: `dimgrey_cavefish_*.bin`

2. **OVMF UEFI Firmware** (`deps/ovmf/`)
   - EDK II OVMF for QEMU testing
   - Copied from system installation or downloaded

3. **Mesa RADV** (`deps/mesa-radv/`) - Planned for Phase 4
   - Vulkan driver for AMD GPUs
   - Will be configured to use custom kernel interface

4. **QT6 Framework** (`deps/qt6/`) - Planned for Phase 7
   - Minimal static build for the single application
   - QtCore, QtGui, QtWidgets modules only

See [deps/README.md](../deps/README.md) for detailed dependency documentation.

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
cd build
cmake --build . --target clean

# Or remove the build directory entirely
cd ..
rm -rf build
mkdir build && cd build
cmake ..
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
