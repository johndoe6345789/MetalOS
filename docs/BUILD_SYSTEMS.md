# MetalOS Build Instructions

This document describes how to build MetalOS using various build systems.

## Prerequisites

### Required Tools

- **Compiler**: GCC 11+ or Clang 13+
- **Assembler**: NASM 2.14+
- **CMake**: 3.16+
- **Ninja** (optional): 1.10+
- **Conan** (optional): 2.0+
- **QEMU**: For testing (with OVMF firmware)

### Install on Ubuntu/Debian

```bash
# Basic build tools
sudo apt-get update
sudo apt-get install -y build-essential cmake nasm

# Ninja build system (optional but faster)
sudo apt-get install -y ninja-build

# QEMU and UEFI firmware for testing
sudo apt-get install -y qemu-system-x86 ovmf

# Conan package manager (optional)
pip3 install conan
```

### Install on Arch Linux

```bash
sudo pacman -S base-devel cmake nasm ninja qemu edk2-ovmf
pip install conan
```

### Install on Fedora

```bash
sudo dnf install gcc gcc-c++ cmake nasm ninja-build qemu edk2-ovmf
pip install conan
```

## Build Methods

### Method 1: CMake with Make (Traditional)

```bash
# Configure
mkdir build && cd build
cmake ..

# Build
make -j$(nproc)

# Build specific targets
make bootloader
make kernel

# Test in QEMU
make image
make qemu
```

### Method 2: CMake with Ninja (Faster)

```bash
# Configure with Ninja
mkdir build && cd build
cmake -G Ninja ..

# Build (much faster than make)
ninja

# Build specific targets
ninja bootloader
ninja kernel

# Test in QEMU
ninja image
ninja qemu
```

### Method 3: Conan + CMake (Modern)

```bash
# Install dependencies (if any are added in future)
conan install . --build=missing

# Build with Conan
conan build .

# Or build with specific profile
conan create . --build=missing
```

### Method 4: Conan + Ninja (Recommended)

```bash
# Configure Conan to use Ninja
conan install . --build=missing -c tools.cmake.cmaketoolchain:generator=Ninja

# Build
conan build .
```

## Build Configuration

### CMake Options

```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (default)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Set QEMU display mode
cmake -DQEMU_DISPLAY=gtk ..
```

### Conan Options

```bash
# With tests (default)
conan build . -o with_tests=True

# Without tests
conan build . -o with_tests=False

# Set QEMU display
conan build . -o qemu_display=gtk
```

## Quick Start

### Fastest Build (Recommended)

```bash
# One-time setup
pip3 install conan
sudo apt-get install -y cmake ninja-build nasm

# Build and test
mkdir build && cd build
cmake -G Ninja ..
ninja
ninja qemu
```

### Simple Build (No extra tools)

```bash
# Just CMake and Make
mkdir build && cd build
cmake ..
make -j$(nproc)
make qemu
```

## Build Targets

### Main Targets

- **all**: Build bootloader and kernel (default)
- **bootloader**: Build UEFI bootloader only
- **kernel**: Build kernel only
- **image**: Create bootable disk image
- **clean**: Clean build artifacts

### Testing Targets

- **test**: Run unit tests
- **qemu**: Run in QEMU with UEFI
- **qemu-debug**: Run with debug output
- **qemu-gdb**: Run with GDB debugging
- **qemu-uefi-test**: Test UEFI boot without OS

## Build Output

```
build/
├── bootloader/
│   └── bootx64.efi          # UEFI bootloader
├── kernel/
│   └── metalos.bin          # Kernel binary
└── metalos.img              # Bootable disk image
```

## Troubleshooting

### NASM not found

```bash
sudo apt-get install nasm
# Or download from https://www.nasm.us/
```

### OVMF firmware not found

```bash
# Ubuntu/Debian
sudo apt-get install ovmf

# Arch Linux
sudo pacman -S edk2-ovmf

# Fedora
sudo dnf install edk2-ovmf
```

### Ninja not found

```bash
sudo apt-get install ninja-build
# Or use make instead: cmake .. (without -G Ninja)
```

### Conan not found

```bash
pip3 install conan
# Or use system package manager
```

## Performance Tips

### Parallel Builds

```bash
# Make (use all CPU cores)
make -j$(nproc)

# Ninja (automatically uses all cores)
ninja

# Conan
conan build . -c tools.cmake.cmake_toolchain:jobs=$(nproc)
```

### Incremental Builds

- Ninja is faster for incremental builds
- Use `ccache` for faster recompilation:
  ```bash
  sudo apt-get install ccache
  export CC="ccache gcc"
  export CXX="ccache g++"
  cmake ..
  ```

### Clean Builds

```bash
# Full clean
rm -rf build/
mkdir build && cd build
cmake ..

# Or use target
make clean       # Make
ninja clean      # Ninja
```

## Cross-Compilation

MetalOS is built for x86_64 bare metal. If you need a cross-compiler:

```bash
# Install x86_64 bare metal toolchain
sudo apt-get install gcc-x86-64-linux-gnu

# Configure CMake to use it
cmake -DCMAKE_C_COMPILER=x86_64-linux-gnu-gcc ..
```

## CI/CD Integration

### GitHub Actions Example

```yaml
- name: Install dependencies
  run: |
    sudo apt-get update
    sudo apt-get install -y cmake ninja-build nasm ovmf qemu-system-x86

- name: Build with Ninja
  run: |
    mkdir build && cd build
    cmake -G Ninja ..
    ninja

- name: Run tests
  run: |
    cd build
    ctest --output-on-failure
```

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [Ninja Build System](https://ninja-build.org/)
- [Conan Documentation](https://docs.conan.io/)
- [NASM Documentation](https://www.nasm.us/doc/)
