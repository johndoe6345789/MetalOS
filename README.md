# MetalOS

**The most minimal OS possible that runs QT6 Hello World.**

Not Linux. Not BSD. Not Windows. Just enough code to boot, initialize GPU, and run one app.

## Extreme Minimalism

MetalOS is an exercise in **absolute minimalism**:

- ❌ **No scheduler** - one app = always running
- ❌ **No process management** - one process only
- ❌ **No file system** - app embedded in boot image
- ❌ **No networking** - not needed
- ❌ **No shell/command line** - boot directly to app
- ❌ **No security** - trust everything
- ❌ **No multi-core** - one CPU core
- ❌ **No dynamic linking** - static link everything

**If it doesn't help QT6 Hello World run, it doesn't exist.**

Target: **< 200 KB OS code** (excluding QT6 itself)

## Project Status

🚧 **In Development** - Phase 1 Complete (Foundation)

Currently: Project structure, documentation, and skeleton code in place.

## What is MetalOS?

MetalOS is **the smallest possible operating system** that can boot QT6 Hello World on specific hardware.

### Philosophy

Every feature must answer: **"Does this help run QT6 Hello World?"**

If no → it doesn't exist.

### Aggressive Minimalism

- **Bootloader**: < 10 KB (just load kernel and jump)
- **Kernel**: < 100 KB (memory, interrupts, drivers)
- **GPU Driver**: < 50 KB (minimal display init)
- **Input Drivers**: < 20 KB (keyboard + mouse only)
- **Total OS**: ~200 KB

Compare to Linux kernel: ~30 MB. We're **150x smaller** by doing only one thing.

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

- **[MINIMALISM.md](docs/MINIMALISM.md)** - ⭐ **START HERE** - Philosophy and what we cut
- **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System design (minimal version)
- **[ROADMAP.md](docs/ROADMAP.md)** - Development phases
- **[BUILD.md](docs/BUILD.md)** - Build instructions
- **[DEVELOPMENT.md](docs/DEVELOPMENT.md)** - Development workflow
- **[COMPONENTS.md](docs/COMPONENTS.md)** - Component checklist
- **[STATUS.md](docs/STATUS.md)** - Current implementation status

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

**Minimalism**: How small can an OS be and still run GUI apps?

**Learning**: Building an OS from scratch is the ultimate systems programming challenge.

**Proof of Concept**: Modern frameworks like QT6 can run on tiny custom OSes.

**Fun**: Because we can.

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

**Golden Rule**: If it doesn't help QT6 Hello World, don't add it.

Contributions welcome for:
- Simplifying existing code (make it smaller!)
- Bug fixes
- GPU driver work (hardest part)
- QT6 port work
- Documentation

Not welcome:
- Adding features "for completeness"
- POSIX compatibility
- Supporting other hardware
- Generalizing anything

See [CONTRIBUTING.md](CONTRIBUTING.md) and [docs/MINIMALISM.md](docs/MINIMALISM.md).

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
