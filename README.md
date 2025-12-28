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

```bash
make all      # Build bootloader, kernel, and userspace
make test     # Run unit tests
make qemu     # Test in QEMU
make clean    # Clean build artifacts
```

See [docs/BUILD.md](docs/BUILD.md) for detailed build instructions.

## Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture and design
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

## License

See [LICENSE](LICENSE) file for details.
