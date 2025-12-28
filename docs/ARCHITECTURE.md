# MetalOS Architecture

## Overview

MetalOS is a ground-up operating system designed specifically to run QT6 applications on AMD64 hardware with UEFI boot and Radeon RX 6600 GPU support.

## Design Principles

1. **Absolute Minimum**: If it's not needed for QT6 Hello World, it doesn't exist
2. **UEFI Native**: Boot directly via UEFI, no legacy BIOS support
3. **Hardware Specific**: AMD64 + Radeon RX 6600 only - no abstractions for other hardware
4. **Single Purpose**: One app, one GPU, one goal - nothing else matters

## System Architecture

```
┌─────────────────────────────────────┐
│     QT6 Hello World Application     │
├─────────────────────────────────────┤
│          QT6 Framework              │
├─────────────────────────────────────┤
│      User Space Runtime             │
├─────────────────────────────────────┤
│       System Call Interface         │
├═════════════════════════════════════┤ ← Kernel Space
│        MetalOS Kernel               │
│  ┌───────────┬──────────┬────────┐  │
│  │  Memory   │ Scheduler│  I/O   │  │
│  │    Mgr    │          │  Mgr   │  │
│  └───────────┴──────────┴────────┘  │
├─────────────────────────────────────┤
│     Hardware Abstraction Layer      │
│  ┌───────────┬──────────┬────────┐  │
│  │    GPU    │   PCI    │ Input  │  │
│  │   Driver  │   Bus    │ Devices│  │
│  └───────────┴──────────┴────────┘  │
├─────────────────────────────────────┤
│         UEFI Boot Services          │
├─────────────────────────────────────┤
│        AMD64 Hardware               │
│  (CPU + Radeon RX 6600 GPU)         │
└─────────────────────────────────────┘
```

## Core Components

### 1. UEFI Bootloader
- **Purpose**: Initialize hardware and load kernel
- **Language**: C with inline assembly
- **Responsibilities**:
  - Set up memory map
  - Initialize graphics framebuffer
  - Load kernel into memory
  - Transfer control to kernel

### 2. Kernel Core
- **Purpose**: Absolute bare minimum OS services
- **Language**: C with assembly for critical parts
- **Subsystems**:
  - Memory Management (just enough for app + QT6)
  - Single process support (no scheduler needed!)
  - Interrupt handling (only what GPU/input needs)
  - Direct syscall to kernel functions (no table/dispatch overhead)

### 3. Hardware Abstraction Layer (HAL)
- **Purpose**: Abstract hardware specifics
- **Components**:
  - PCI enumeration and configuration
  - GPU driver (Radeon RX 6600 specific)
  - Framebuffer management
  - Input device drivers

### 4. User Space Runtime
- **Purpose**: Support the single QT6 application
- **Features**:
  - Application loader (ELF format, static-linked)
  - C/C++ standard library subset
  - QT6 framework (statically linked into application)
  - No shell, no command line - direct boot to app

## Memory Layout (Simplified)

```
0x0000000000000000 - 0x0000000000000FFF : NULL guard page
0x0000000000001000 - 0x00000000000FFFFF : Bootloader (temporary)
0x0000000000100000 - 0x0000000000FFFFFF : Kernel (small!)
0x0000000001000000 - 0x00000000FFFFFFFF : Application + QT6
GPU VRAM: Separate, mapped via BAR
```

No complex memory regions - keep it simple!

## Boot Process (Minimal)

1. **UEFI Firmware** loads bootloader
2. **Bootloader** gets framebuffer, loads kernel, jumps
3. **Kernel** maps memory, enables interrupts
4. **Kernel** initializes GPU (minimal)
5. **Kernel** sets up input (keyboard/mouse only)
6. **Kernel** jumps directly to QT6 app
7. **App** runs until halt

**That's it. No filesystem, no daemons, no services, no nothing.**

## Development Phases

### Phase 1: Foundation (Current)
- Project structure
- Build system
- Basic documentation
- Minimal bootloader stub

### Phase 2: Kernel Basics
- Boot to kernel C code
- Basic console output
- Memory management
- Interrupt handling

### Phase 3: Hardware Support
- PCI enumeration
- Basic GPU initialization
- Framebuffer graphics
- Keyboard/mouse input

### Phase 4: User Space
- System call interface
- Process loading
- QT6 dependencies
- User space runtime

### Phase 5: QT6 Integration
- Port QT6 to MetalOS
- Build Hello World app
- Full screen rendering
- Final testing

## Technical Decisions

### Why UEFI?
- Modern standard for booting
- Better hardware discovery
- Graphics mode setup easier
- No legacy baggage

### Why C/C++?
- Direct hardware access
- Good toolchain support
- QT6 is C++ based
- Industry standard for OS development

### Why Radeon RX 6600 Specific?
- Define clear target for initial implementation
- Avoid generic driver complexity
- Can expand later if needed

### Why QT6?
- Modern cross-platform framework
- Good graphics support
- Active development
- Clear end goal

## References

- UEFI Specification 2.10
- AMD64 Architecture Programmer's Manual
- AMD Radeon GPU Documentation
- QT6 Documentation
- Linux Kernel (for driver inspiration)
