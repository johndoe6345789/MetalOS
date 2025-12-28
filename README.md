# MetalOS

**An extremely minimalist OS - One app, one purpose, zero cruft.**

MetalOS boots directly to a single full-screen QT6 application. No shell, no filesystem, no complexity.

## Philosophy

> *"If it doesn't help QT6 Hello World, it doesn't exist."*

This OS exists solely to run **one QT6 application** on **AMD64 + Radeon RX 6600** hardware. Everything else is cut.

## Design Principles

✅ **Boot directly to app** - No command line or shell  
✅ **Single application** - One process, always running  
✅ **No filesystem** - App embedded in boot image  
✅ **Static linking only** - Maximum simplicity  
✅ **Creative freedom** - Not bound by POSIX or tradition  
✅ **Precise drivers** - Hardware code follows specs exactly

## GPU Implementation Strategy

MetalOS leverages Mesa RADV (userspace Vulkan driver) with a minimal kernel-side GPU API to achieve high performance without excessive complexity. The strategy focuses on implementing only the essential kernel interfaces that RADV requires:

- **Firmware loading** and ASIC initialization for Navi 23
- **Buffer objects** (VRAM/GTT management)
- **Virtual memory** (GPU page tables)
- **Command submission** (rings/queues) and synchronization primitives

This approach keeps the OS non-POSIX while avoiding the complexity of writing a Vulkan driver from scratch.

For detailed implementation notes, see [docs/GPU_IMPLEMENTATION.md](docs/GPU_IMPLEMENTATION.md).


## What We Cut

See [docs/MINIMALISM.md](docs/MINIMALISM.md) for full philosophy.

❌ Scheduler ❌ Process management ❌ Filesystem  
❌ Networking ❌ Security ❌ Multi-core  
❌ Dynamic linking ❌ ACPI ❌ Virtual memory complexity

## What We Keep (Absolute Minimum)

✅ Memory allocator (bump allocator)  
✅ ~5 interrupt handlers (timer, keyboard, mouse, GPU)  
✅ GPU driver (framebuffer only, ~50 KB)  
✅ Input drivers (PS/2 first, USB fallback, ~20 KB)  
✅ PCI scan (just find our GPU)  
✅ ~5 syscalls (write, mmap, ioctl, poll, exit)

**Target OS Size**: ~200 KB (excluding QT6)

## Development Phases

**Phase 1: Project Foundation** ✅ COMPLETE  
**Phase 2: UEFI Bootloader** (Next)  
**Phase 3: Core Kernel Components**  
**Phase 4: Hardware Support**  
**Phase 5: System Call Interface**  
**Phase 6: User Space & Application**  
**Phase 7: QT6 Port**  
**Phase 8: Integration & Testing**

See [docs/ROADMAP.md](docs/ROADMAP.md) for detailed phase breakdown.

## Building

MetalOS uses **CMake** as its build system for a modern, cross-platform build experience.

### Quick Start (CMake)

```bash
mkdir build && cd build
cmake ..
cmake --build .
cmake --build . --target qemu
```

### Using Ninja (Faster Builds)

```bash
mkdir build && cd build
cmake -G Ninja ..
ninja
ninja qemu
```

### Conan (With Package Management)

```bash
# First time: install Conan and setup profile
pip3 install conan
conan profile detect --force

# Install dependencies and generate toolchain (Release build by default)
conan install . --build=missing

# Configure and build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../build/Release/generators/conan_toolchain.cmake -G Ninja
ninja

# Note: For Debug build, use:
# conan install . --build=missing -s build_type=Debug
# cmake .. -DCMAKE_TOOLCHAIN_FILE=../build/Debug/generators/conan_toolchain.cmake -G Ninja
```

### Docker Build (Recommended for Consistency)

The easiest way to build MetalOS with all dependencies:

```bash
./scripts/docker-build.sh              # Build Docker image
./scripts/docker-run.sh scripts/setup-deps.sh  # Setup dependencies
./scripts/docker-run.sh cmake --build build    # Build everything
```

**QEMU UEFI Testing**:
```bash
cmake --build . --target qemu          # Boot in QEMU with UEFI (headless)
cmake --build . --target qemu-debug    # Boot with debug output
cmake --build . --target qemu-gdb      # Boot with GDB debugging
cmake --build . --target qemu-uefi-test # Test UEFI firmware setup
```

See [docs/BUILD.md](docs/BUILD.md) for detailed build instructions and [docs/TESTING.md](docs/TESTING.md) for testing guide.

## Dependencies

MetalOS manages third-party dependencies in-house for reproducibility and offline development:

- **GPU Firmware** - AMD Navi 23 firmware blobs (dimgrey_cavefish_*.bin)
- **Mesa RADV** - Vulkan driver for AMD GPUs (planned Phase 4)
- **QT6** - Application framework (minimal static build, planned Phase 7)
- **OVMF** - UEFI firmware for QEMU testing

**Setup dependencies**: `./scripts/setup-deps.sh all`

See [deps/README.md](deps/README.md) for detailed dependency management instructions.

## Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture and design
- [GPU_IMPLEMENTATION.md](docs/GPU_IMPLEMENTATION.md) - GPU driver strategy and implementation
- [MINIMALISM.md](docs/MINIMALISM.md) - Extreme minimalism philosophy
- [ROADMAP.md](docs/ROADMAP.md) - Development phases and milestones
- [BUILD.md](docs/BUILD.md) - Build system and toolchain
- [DEVELOPMENT.md](docs/DEVELOPMENT.md) - Development environment setup
- [STATUS.md](docs/STATUS.md) - Current implementation status
- [TESTING.md](docs/TESTING.md) - Unit tests and QEMU testing

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines. Remember: simplicity over features.

## Target Hardware

- **CPU**: AMD64 (x86-64)
- **GPU**: Radeon RX 6600
- **Input**: PS/2 or USB keyboard/mouse
- **Boot**: UEFI
- **QEMU (UEFI MODE)**: For testing the OS.

## License

See [LICENSE](LICENSE) file for details.
