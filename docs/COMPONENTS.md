# MetalOS - Planned Components

This document outlines the key components that need to be implemented.

## Bootloader Components

### Implemented
- [x] Basic structure
- [x] EFI types and definitions
- [x] Entry point stub

### TODO
- [ ] UEFI protocol implementations
  - [ ] Console I/O (OutputString, etc.)
  - [ ] Graphics Output Protocol
  - [ ] Simple File System Protocol
  - [ ] Memory allocation
- [ ] Graphics initialization
  - [ ] Mode enumeration
  - [ ] Mode selection
  - [ ] Framebuffer setup
- [ ] Kernel loading from disk
- [ ] Memory map retrieval
- [ ] Exit boot services
- [ ] Jump to kernel with boot info

## Kernel Components

### Core Kernel

#### Implemented
- [x] Entry point
- [x] Basic console output to framebuffer
- [x] Boot info structure

#### TODO
- [ ] GDT (Global Descriptor Table) setup
- [ ] IDT (Interrupt Descriptor Table) setup
- [ ] Interrupt handlers (ISRs)
- [ ] Exception handlers
- [ ] Timer interrupt (for scheduling)

### Memory Management

#### TODO
- [ ] Physical memory allocator
  - [ ] Parse UEFI memory map
  - [ ] Bitmap or buddy allocator
  - [ ] Page allocation (4KB pages)
- [ ] Virtual memory
  - [ ] Page table setup (4-level paging)
  - [ ] Kernel space mapping
  - [ ] User space mapping
- [ ] Heap allocator
  - [ ] Simple malloc/free
  - [ ] For kernel use

### Process Management

#### TODO
- [ ] Process structure
- [ ] Thread structure
- [ ] Context switching
  - [ ] Save/restore CPU state
  - [ ] Stack switching
- [ ] Simple scheduler
  - [ ] Round-robin
  - [ ] Single user process initially
- [ ] User/kernel mode transitions

### System Calls

#### TODO
- [ ] System call mechanism (syscall/sysret)
- [ ] System call table
- [ ] Essential syscalls:
  - [ ] exit()
  - [ ] write() - for debugging
  - [ ] mmap() - memory allocation
  - [ ] ioctl() - device control
  - [ ] poll() - wait for events

## Hardware Abstraction Layer (HAL)

### PCI Bus

#### TODO
- [ ] PCI configuration space access
- [ ] Device enumeration
- [ ] Find GPU (vendor 0x1002, device ID for RX 6600)
- [ ] BAR (Base Address Register) reading
- [ ] Enable memory and I/O access

### GPU Driver (Radeon RX 6600)

This needs to be precise - based on actual AMD GPU specs.

#### TODO
- [ ] GPU identification
  - [ ] Check PCI vendor/device ID
  - [ ] Verify it's RX 6600 (Navi 23)
- [ ] Basic initialization
  - [ ] Map MMIO registers
  - [ ] Reset GPU if needed
  - [ ] Initialize display engine
- [ ] Display pipeline setup
  - [ ] Configure CRTC (display controller)
  - [ ] Set up display timing
  - [ ] Configure framebuffer
  - [ ] Enable output
- [ ] Framebuffer management
  - [ ] Allocate VRAM for framebuffer
  - [ ] Map to CPU address space
  - [ ] Set up for blitting

**Note**: This is the most complex part. Will need to reference:
- Linux amdgpu driver
- AMD register specifications
- Display Controller (DCN) documentation

### Input Devices

#### TODO
- [ ] USB Stack (minimal)
  - [ ] XHCI controller initialization
  - [ ] USB device enumeration
  - [ ] HID class driver
- [ ] Keyboard driver
  - [ ] USB HID keyboard
  - [ ] Key code translation
  - [ ] Event generation for QT
- [ ] Mouse driver
  - [ ] USB HID mouse
  - [ ] Movement/button tracking
  - [ ] Event generation for QT

### Timer

#### TODO
- [ ] APIC timer or PIT
- [ ] Timer interrupt setup
- [ ] Time tracking for scheduler

## User Space

### Runtime

#### TODO
- [ ] ELF loader
  - [ ] Parse ELF64 headers
  - [ ] Load program segments
  - [ ] Set up entry point
- [ ] C runtime
  - [ ] _start function
  - [ ] Call constructors
  - [ ] Call main()
  - [ ] Call destructors
- [ ] C++ runtime
  - [ ] Global constructors
  - [ ] Global destructors
  - [ ] Exception handling (minimal or disabled)
- [ ] Standard library subset
  - [ ] malloc/free
  - [ ] memcpy/memset/memcmp
  - [ ] String functions
  - [ ] Basic I/O

### Application Launcher (No Init Needed)

#### TODO
- [ ] Direct kernel boot into application
- [ ] Load QT6 hello world as only user process
- [ ] No shell, no command line
- [ ] Handle application exit (just halt or reboot)

## QT6 Port

### Dependencies

#### TODO
- [ ] Identify minimal QT6 dependencies
- [ ] Port/stub required libraries:
  - [ ] zlib
  - [ ] libpng
  - [ ] freetype (fonts)
  - [ ] Others as discovered

### QT Platform Abstraction (QPA)

#### TODO
- [ ] Create MetalOS QPA plugin
- [ ] Implement platform integration
  - [ ] Window system integration (full screen)
  - [ ] Event dispatcher
  - [ ] Backingstore (framebuffer backing)
- [ ] Graphics backend
  - [ ] Raster rendering to framebuffer
  - [ ] Optionally: GPU acceleration
- [ ] Input integration
  - [ ] Keyboard events
  - [ ] Mouse events
  - [ ] Event queue
- [ ] Font backend
  - [ ] Basic font rendering
  - [ ] Use embedded font or simple bitmap

### QT Build

#### TODO
- [ ] Configure QT6 for MetalOS target
- [ ] Disable unnecessary modules
- [ ] Enable: QtCore, QtGui, QtWidgets only
- [ ] Cross-compile for MetalOS
- [ ] Static linking

## Applications

### Hello World

#### Implemented
- [x] Basic QT6 hello world source code
- [x] .pro file

#### TODO
- [ ] Build for MetalOS
- [ ] Static link with QT6
- [ ] Test rendering
- [ ] Test input handling
- [ ] Package into bootable image

## Build System

#### Implemented
- [x] Basic Makefiles
- [x] Directory structure

#### TODO
- [ ] Cross-compiler setup scripts
- [ ] Image creation script
  - [ ] Create GPT disk image
  - [ ] Format EFI System Partition
  - [ ] Copy bootloader and kernel
- [ ] Automated testing in QEMU
- [ ] CI/CD pipeline (optional)

## Documentation

#### Implemented
- [x] README.md
- [x] ARCHITECTURE.md
- [x] BUILD.md
- [x] DEVELOPMENT.md
- [x] ROADMAP.md
- [x] CONTRIBUTING.md

#### TODO
- [ ] API documentation
- [ ] Driver development guide
- [ ] Troubleshooting guide
- [ ] Performance tuning guide

## Testing

#### TODO
- [ ] Unit tests for key components
- [ ] Integration tests
- [ ] QEMU boot tests
- [ ] Hardware test checklist

---

This is a living document - components will be checked off as implemented.
