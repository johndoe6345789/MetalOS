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

### Docker Build (Recommended)

The easiest way to build MetalOS with all dependencies:

```bash
./scripts/docker-build.sh              # Build Docker image
./scripts/docker-run.sh scripts/setup-deps.sh  # Setup dependencies
./scripts/docker-run.sh make all       # Build everything
./scripts/docker-run.sh make qemu      # Test in QEMU
```

### Native Build

```bash
make all      # Build bootloader, kernel, and userspace
make test     # Run unit tests
make qemu     # Test in QEMU with UEFI firmware
make clean    # Clean build artifacts
```

**QEMU UEFI Testing**:
```bash
make qemu                      # Boot in QEMU with UEFI (headless)
make qemu QEMU_DISPLAY=gtk     # Boot with graphical display
make qemu-debug                # Boot with debug output
make qemu-gdb                  # Boot with GDB debugging
make qemu-uefi-test            # Test UEFI firmware setup
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
