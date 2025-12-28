# MetalOS Architecture

## Overview

MetalOS is a ground-up operating system designed specifically to run QT6 applications on AMD64 hardware with UEFI boot and Radeon RX 6600 GPU support.

## Design Principles

1. **Minimal by Design**: Only implement what's necessary to run QT6 applications
2. **UEFI Native**: Boot directly via UEFI, no legacy BIOS support
3. **Hardware Specific**: Optimized for AMD64 + Radeon RX 6600
4. **Modern Approach**: Learn from Linux but implement cleanly from scratch

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
- **Purpose**: Provide essential OS services
- **Language**: C/C++ with assembly for low-level operations
- **Subsystems**:
  - Memory Management (physical/virtual)
  - Process/Thread Scheduler
  - Interrupt Handling
  - System Call Interface

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

## Memory Layout

```
0x0000000000000000 - 0x0000000000000FFF : NULL guard page
0x0000000000001000 - 0x00000000000FFFFF : Bootloader code/data
0x0000000000100000 - 0x00000000FFFFFFFF : Kernel space
0x0000000100000000 - 0x00007FFFFFFFFFFF : User space
0xFFFF800000000000 - 0xFFFFFFFFFFFFFFFF : Kernel heap/stacks
```

## Boot Process

1. **UEFI Firmware** loads bootloader from EFI System Partition
2. **Bootloader** initializes basic hardware and sets up memory
3. **Bootloader** locates and loads kernel binary
4. **Bootloader** exits UEFI boot services
5. **Kernel** initializes subsystems (memory, scheduler, interrupts)
6. **Kernel** loads HAL drivers (GPU, PCI, input)
7. **Kernel** directly launches QT6 Hello World application (no shell, no init)
8. **Application** runs full-screen until exit/reboot

**Note**: No command line, no shell - the system boots directly into the single application.

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
