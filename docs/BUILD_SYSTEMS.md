# MetalOS Build Systems Guide

MetalOS uses **CMake** as its primary build system, which can be used with different build backends for different workflows.

## Quick Start

### Using CMake (Default)
```bash
mkdir build && cd build
cmake ..
cmake --build .
cmake --build . --target qemu  # Test in QEMU
```

### Using CMake + Ninja (Fastest)
```bash
mkdir build && cd build
cmake -G Ninja ..
ninja
ninja qemu  # Test in QEMU
```

### Using Conan + CMake
```bash
# First time: install Conan and setup profile
pip3 install conan
conan profile detect --force

# Install dependencies (generates toolchain files)
conan install . --build=missing

# Configure with Conan toolchain
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../build/Release/generators/conan_toolchain.cmake

# Build
cmake --build .
```

## Build System Comparison

| Build Backend | Speed | Features | Best For |
|---------------|-------|----------|----------|
| **Make** (default) | Medium | Cross-platform, standard | General use, CI/CD |
| **Ninja** | Fast | Parallel builds | Development, large projects |
| **Conan** | Medium | Dependency management | Projects with external deps |

## Detailed Usage

### 1. CMake (Primary Build System)

CMake is the primary build system for MetalOS, providing cross-platform support and modern features.

#### Build Commands
```bash
# Configure and build
mkdir build && cd build
cmake ..
cmake --build .

# Build specific targets
cmake --build . --target bootloader_efi
cmake --build . --target kernel_bin
cmake --build . --target image

# Create bootable image
cmake --build . --target image

# Run in QEMU
cmake --build . --target qemu          # Headless mode
cmake --build . --target qemu-debug    # With debug output
cmake --build . --target qemu-gdb      # With GDB server
cmake --build . --target qemu-uefi-test # Test UEFI setup

# Test
ctest
ctest --output-on-failure
ctest -V  # Verbose

# Install
cmake --install .
cmake --install . --prefix /path/to/install

# Clean
cmake --build . --target clean
rm -rf build  # Complete clean
```

#### Advantages
- ✅ Cross-platform (Windows, Linux, macOS)
- ✅ IDE integration (CLion, VSCode, Visual Studio)
- ✅ Modern dependency management
- ✅ Better parallel build support
- ✅ Generates compile_commands.json for IDEs

#### Disadvantages
- ❌ Requires cmake installation
- ❌ Slightly more complex setup

---

### 2. Ninja (Fast Build Backend)

Ninja is a fast build backend that can be used with CMake for faster incremental builds.

#### Advantages
- ✅ Cross-platform (Windows, Linux, macOS)
- ✅ IDE integration (CLion, Visual Studio, VS Code)
- ✅ Modern and widely adopted
- ✅ Better dependency tracking
- ✅ Supports multiple generators

#### Disadvantages
- ❌ More complex than Make
- ❌ Requires CMake to be installed
- ❌ Learning curve for CMakeLists.txt syntax

---

### 3. Ninja (Fast Build System)

Ninja is designed for speed and is often used with CMake.

#### Build Commands
```bash
# Configure with Ninja generator
mkdir build && cd build
cmake -G Ninja ..

# Build (much faster than Make)
ninja

# Build specific targets
ninja bootloader_efi
ninja kernel_bin
ninja image
ninja qemu

# Clean
ninja clean
```

#### Advantages
- ✅ **Very fast** - optimized for speed
- ✅ Better parallelization than Make
- ✅ Accurate dependency tracking
- ✅ Clean output format
- ✅ Works great with CMake

#### Disadvantages
- ❌ Requires Ninja to be installed
- ❌ Less familiar than Make
- ❌ Requires CMake to generate build files

---

### 4. Conan (Package Manager + Build System)

Conan manages dependencies and integrates with CMake.

#### Setup
```bash
# Install Conan (first time only)
pip install conan

# Initialize Conan profile (first time only)
conan profile detect --force
```

#### Build Commands
```bash
# Install dependencies (generates toolchain in build/Release/generators/)
conan install . --build=missing

# Alternative: Install with specific settings
conan install . --build=missing -s build_type=Debug
conan install . --build=missing -s build_type=Release

# Create build directory and configure with Conan-generated toolchain
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../build/Release/generators/conan_toolchain.cmake

# Build
cmake --build .

# Or use Conan to build directly
cd ..
conan build . --build-folder=build
```

#### Advantages
- ✅ Dependency management (for future QT6, Mesa, etc.)
- ✅ Reproducible builds
- ✅ Version management
- ✅ Cross-platform package management
- ✅ Integration with CMake and other build systems
- ✅ **Recommended for CI/CD** - ensures consistent builds

#### Disadvantages
- ❌ Requires Python and Conan
- ❌ Additional setup step
- ❌ Learning curve

#### When to Use
- ✅ **CI/CD pipelines** (all workflows now use Conan)
- ✅ **Production builds** requiring reproducibility
- ✅ **When adding external dependencies** (QT6, Mesa RADV in the future)
- ✅ **Cross-platform development** needing consistent dependencies

---

## Which Build System Should I Use?

### For Quick Development
**Use: Make or CMake + Ninja**
```bash
# Make - simplest
make all && make qemu

# Or Ninja - fastest
cd build-ninja && ninja && ninja qemu
```

### For IDE Integration
**Use: CMake**
- Works with CLion, Visual Studio Code, Visual Studio
- Configure your IDE to use the CMakeLists.txt

### For CI/CD
**Use: Conan + CMake (Recommended)**
```bash
# GitHub Actions, GitLab CI, etc.
# All MetalOS CI workflows now use Conan
conan install . --build=missing
cmake -B build -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake
cmake --build build
ctest --test-dir build
```

Benefits:
- ✅ Reproducible builds across different CI runners
- ✅ Consistent dependency versions
- ✅ Future-proof for when we add dependencies

### For Cross-Platform Development
**Use: Conan + CMake + Ninja**
```bash
# Works on Linux, macOS, Windows
conan install . --build=missing
cmake -G Ninja -B build -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake
cmake --build build
```

### For Projects with Dependencies (Current & Future)
**Use: Conan + CMake**
```bash
# When we add QT6, Mesa RADV, etc.
mkdir build && cd build
conan install .. --build=missing
cmake .. -DCMAKE_TOOLCHAIN_FILE=../build/Release/generators/conan_toolchain.cmake
cmake --build .
```

---

## File Structure

```
MetalOS/
├── Makefile                  # Traditional Make build
├── CMakeLists.txt            # Root CMake configuration
├── conanfile.py              # Conan package definition
├── conanfile.txt             # Simple Conan configuration
├── bootloader/
│   ├── Makefile              # Bootloader Make build
│   └── CMakeLists.txt        # Bootloader CMake build
├── kernel/
│   ├── Makefile              # Kernel Make build
│   └── CMakeLists.txt        # Kernel CMake build
└── tests/
    ├── Makefile              # Tests Make build
    └── CMakeLists.txt        # Tests CMake build
```

---

## Common Build Options

### CMake Options
```bash
# Enable/disable components
-DBUILD_BOOTLOADER=ON/OFF
-DBUILD_KERNEL=ON/OFF
-DBUILD_TESTS=ON/OFF

# Build type
-DCMAKE_BUILD_TYPE=Debug
-DCMAKE_BUILD_TYPE=Release
-DCMAKE_BUILD_TYPE=RelWithDebInfo

# Compiler
-DCMAKE_C_COMPILER=/usr/bin/gcc
-DCMAKE_CXX_COMPILER=/usr/bin/g++

# Install prefix
-DCMAKE_INSTALL_PREFIX=/opt/metalos
```

### Make Options
```bash
# Display mode for QEMU
QEMU_DISPLAY=gtk
QEMU_DISPLAY=sdl
QEMU_DISPLAY=none

# Verbose output
V=1
VERBOSE=1
```

---

## Troubleshooting

### CMake: Ninja not found
```bash
# Ubuntu/Debian
sudo apt-get install ninja-build

# macOS
brew install ninja

# Arch Linux
sudo pacman -S ninja
```

### CMake: OVMF not found
```bash
# Ubuntu/Debian
sudo apt-get install ovmf

# Arch Linux
sudo pacman -S edk2-ovmf

# Or specify manually
cmake .. -DOVMF_FIRMWARE=/path/to/OVMF.fd
```

### Conan: Profile not found
```bash
# Detect default profile
conan profile detect --force

# Create custom profile
conan profile show default > myprofile
# Edit myprofile as needed
conan install .. --profile=myprofile
```

---

## Performance Comparison

Build time for clean build (approximate):

| Build System | Time | Notes |
|--------------|------|-------|
| Make         | ~2s  | Single-threaded by default |
| Make -j8     | ~1s  | With 8 parallel jobs |
| CMake + Make | ~2s  | Same as Make |
| CMake + Ninja| ~0.5s| Fastest option |
| Conan + CMake| ~3s  | Additional dependency resolution |

*Times measured on a typical development machine. Your results may vary.*

---

## Future Plans

As MetalOS grows and adds dependencies (QT6, Mesa RADV), we recommend:

1. **Development**: CMake + Ninja (fastest iteration)
2. **Dependency Management**: Conan (when dependencies are added)
3. **CI/CD**: Keep Make for simplicity, or migrate to CMake
4. **Distribution**: Create Conan packages for easy installation

---

## Getting Help

- Check `docs/BUILD.md` for detailed build instructions
- See `docs/DEVELOPMENT.md` for development workflow
- Read individual Makefiles and CMakeLists.txt for specifics
- Check `conanfile.py` comments for dependency information

---

**Note**: All build systems produce identical outputs. Choose based on your workflow and preferences!
