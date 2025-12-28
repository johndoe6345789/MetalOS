# MetalOS

A minimal operating system built from the ground up to run QT6 applications on AMD64 hardware with Radeon RX 6600 GPU and UEFI boot.

**Not Linux. Not BSD. Not Windows.** Just enough OS to run a QT6 Hello World full-screen application.

## Project Status

🚧 **In Development** - Phase 1 Complete (Foundation)

Currently: Project structure, documentation, and skeleton code in place.

## What is MetalOS?

MetalOS is a **minimal, purpose-built operating system** with a single goal: demonstrate that you can build a custom OS from scratch to run modern GUI applications (specifically QT6).

### Key Features

- ✅ **UEFI Native**: Modern boot via UEFI (no legacy BIOS)
- ✅ **AMD64 Architecture**: 64-bit x86 processor support
- ✅ **Radeon RX 6600 GPU**: Hardware-specific graphics support
- ✅ **QT6 Framework**: Full QT6 widget support
- ✅ **Minimal Design**: Only what's necessary, nothing more

### What's Included

- **UEFI Bootloader**: Initializes hardware and loads kernel
- **Minimal Kernel**: Memory management, scheduling, interrupts
- **GPU Driver**: Radeon RX 6600 specific (inspired by Linux amdgpu)
- **HAL**: Hardware abstraction for PCI, input devices
- **User Space**: Minimal C++ runtime for applications
- **QT6 Port**: QT6 framework ported to MetalOS
- **Hello World App**: Full-screen QT6 demonstration application

### What's NOT Included

- ❌ Multi-user support
- ❌ Command line / shell
- ❌ Networking stack
- ❌ File systems (app embedded in boot image)
- ❌ POSIX compatibility
- ❌ Security features (this is a demo/learning project)
- ❌ Support for other hardware
- ❌ Multiple applications (single app only)

## Quick Start

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install build-essential nasm qemu-system-x86 ovmf

# macOS
brew install nasm qemu
```

You'll also need a cross-compiler. See [docs/BUILD.md](docs/BUILD.md) for details.

### Building

```bash
# Clone the repository
git clone https://github.com/johndoe6345789/MetalOS.git
cd MetalOS

# Build everything
make all

# Create bootable image
make image

# Test in QEMU
make qemu
```

### Testing on Hardware

⚠️ **WARNING**: This is experimental software. Test on non-production hardware only.

See [docs/BUILD.md](docs/BUILD.md) for instructions on creating a bootable USB drive.

## Documentation

- **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System design and architecture
- **[BUILD.md](docs/BUILD.md)** - Build instructions and dependencies
- **[DEVELOPMENT.md](docs/DEVELOPMENT.md)** - Development workflow and guidelines
- **[ROADMAP.md](docs/ROADMAP.md)** - Development phases and timeline

## Project Structure

```
MetalOS/
├── bootloader/        # UEFI bootloader
├── kernel/           # MetalOS kernel
├── userspace/        # User space runtime and applications
│   ├── runtime/     # C++ runtime
│   ├── init/        # Init process
│   └── apps/        # QT6 hello world application
├── docs/            # Documentation
└── scripts/         # Build scripts
```

## Development Phases

- [x] **Phase 1**: Foundation (Complete)
- [ ] **Phase 2**: UEFI Bootloader
- [ ] **Phase 3**: Minimal Kernel
- [ ] **Phase 4**: Hardware Abstraction Layer
- [ ] **Phase 5**: System Call Interface
- [ ] **Phase 6**: User Space Runtime
- [ ] **Phase 7**: QT6 Port
- [ ] **Phase 8**: Integration & Polish

See [docs/ROADMAP.md](docs/ROADMAP.md) for detailed breakdown.

## Why?

**Learning**: Building an OS from scratch is the ultimate systems programming education.

**Minimal Design**: Modern OSes are complex. This project asks: "What's the absolute minimum needed for a GUI application?"

**Custom Hardware**: Show that you can optimize an OS for specific hardware instead of supporting everything.

**QT6 Demo**: Prove that modern application frameworks can run on custom OS implementations.

## Technology Stack

- **Language**: C for kernel, C++ for applications
- **Boot**: UEFI
- **Architecture**: AMD64 (x86-64)
- **Graphics**: Direct framebuffer + Radeon RX 6600
- **GUI**: QT6 (Core, Gui, Widgets)
- **Build**: GNU Make, GCC cross-compiler

## Inspiration

- **Linux Kernel**: For driver implementations (especially GPU)
- **SerenityOS**: For clean, minimal OS design
- **TempleOS**: For single-purpose OS philosophy
- **Redox OS**: For Rust-based OS architecture (though we use C/C++)

## Contributing

This is primarily a learning/demonstration project, but contributions are welcome!

**Guidelines**:
- Keep it minimal - every feature must justify its existence
- Document your changes
- Follow existing code style
- Test on QEMU before submitting

See [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md) for details.

## License

See [LICENSE](LICENSE) file for details.

## Disclaimer

⚠️ **This is not production software!** MetalOS is a learning/demonstration project. It lacks security features, error handling, and hardware support expected in production OSes.

Do not use for anything important!

## Contact

- **Issues**: [GitHub Issues](https://github.com/johndoe6345789/MetalOS/issues)
- **Discussions**: [GitHub Discussions](https://github.com/johndoe6345789/MetalOS/discussions)

---

**MetalOS** - A minimal OS for maximal learning.
