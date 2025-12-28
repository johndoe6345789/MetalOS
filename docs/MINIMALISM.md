# MetalOS - Extreme Minimalism Guide

## What ABSOLUTE MINIMUM Means

We're not building a general-purpose OS. We're building **the smallest possible software layer** that lets QT6 Hello World run on AMD64 + RX 6600.

## Cut Everything

### ❌ NO Scheduler
- **Why not?**: One app = one process = always running
- **Instead**: Direct execution, no context switching
- **Saves**: Scheduler code, process queues, timer interrupts for scheduling

### ❌ NO Process Management
- **Why not?**: Only one process ever exists
- **Instead**: App runs in ring 0 (kernel mode) or simple ring 3 with static setup
- **Saves**: Process structures, fork/exec, process lifecycle

### ❌ NO File System
- **Why not?**: Everything loaded at boot
- **Instead**: App binary embedded in boot image or loaded by bootloader once
- **Saves**: VFS, inode structures, file descriptors, read/write/open/close

### ❌ NO Networking
- **Why not?**: Hello World doesn't need network
- **Instead**: Nothing
- **Saves**: TCP/IP stack, network drivers, sockets

### ❌ NO Security
- **Why not?**: Single app, demo system
- **Instead**: Trust everything
- **Saves**: Permissions, user/group IDs, capabilities, security modules

### ❌ NO Virtual Memory (maybe)
- **Why not?**: Could run everything with identity mapping
- **Instead**: Simple page tables just to keep CPU happy
- **Saves**: TLB management, page fault handling, swapping

### ❌ NO Dynamic Linking
- **Why not?**: Complexity nightmare
- **Instead**: Static link everything into one blob
- **Saves**: ELF loader complexity, PLT/GOT, symbol resolution at runtime

### ❌ NO Multi-core
- **Why not?**: One app on one core is plenty
- **Instead**: Use only bootstrap processor (BSP)
- **Saves**: SMP initialization, locks, atomic operations, IPI handling

### ❌ NO ACPI (mostly)
- **Why not?**: Complex and usually not needed for basic operation
- **Instead**: Minimal ACPI just for GPU if absolutely required
- **Saves**: AML interpreter, ACPI tables parsing, power management

### ❌ NO Signals/IPC
- **Why not?**: One process = nobody to talk to
- **Instead**: Nothing
- **Saves**: Signal handling, pipes, message queues, shared memory

### ❌ NO Timers (mostly)
- **Why not?**: QT6 needs *some* time, but minimal
- **Instead**: Simple timer for QT event loop only
- **Saves**: Complex timer infrastructure, high-resolution timers

## What We MUST Keep (Absolute Minimum)

### ✅ Memory Allocator
- **Why**: QT6 needs malloc/free
- **Minimum**: Simple bump allocator or basic free list
- **No need for**: Fancy algorithms, defragmentation, memory pools

### ✅ Interrupts
- **Why**: Keyboard/mouse input, timer for QT
- **Minimum**: 
  - Keyboard interrupt
  - Mouse interrupt  
  - Timer interrupt (for QT event loop)
  - GPU interrupt (if needed)
- **That's it!** Maybe 4-5 interrupt handlers total

### ✅ GPU Driver
- **Why**: Need to display graphics
- **Minimum**:
  - Initialize display pipeline
  - Set up framebuffer
  - Blit pixels to screen
- **No need for**: 3D acceleration, multiple displays, hot-plug

### ✅ Input Drivers
- **Why**: Need keyboard/mouse for QT events
- **Minimum**:
  - USB XHCI (or PS/2 if simpler!)
  - HID keyboard
  - HID mouse
- **No need for**: Other USB devices, complex HID parsing

### ✅ PCI Enumeration
- **Why**: Find the GPU
- **Minimum**: 
  - Scan bus for our specific GPU
  - Enable memory/IO access
- **No need for**: Full PCI tree, hot-plug, PCI-to-PCI bridges

### ✅ System Calls (bare minimum)
- **Why**: User space needs to talk to kernel
- **Minimum**:
  - `write()` - for debugging output
  - `mmap()` - for memory
  - `ioctl()` - for GPU/input
  - Maybe 5-10 syscalls total
- **No need for**: POSIX compatibility, 300+ syscalls

## Radical Simplifications

### Framebuffer Over Everything
- **Skip**: Complex GPU 3D acceleration
- **Use**: Direct framebuffer blitting
- **QT6**: Can render to memory, we copy to VRAM
- **Trade-off**: Slower but WAY simpler

### Identity Mapping (maybe)
- **Skip**: Complex virtual memory
- **Use**: Physical = Virtual (or simple offset)
- **Note**: x86-64 requires paging enabled, but can be trivial

### PS/2 Over USB
- **Skip**: Complex USB stack if possible
- **Use**: PS/2 keyboard/mouse if hardware supports
- **Simpler**: Direct port I/O vs USB protocol

### Polling Over Interrupts (where possible)
- **Skip**: Complex interrupt handling
- **Use**: Poll for events in tight loop
- **Trade-off**: CPU usage vs complexity

### Hardcode Everything
- **Skip**: Configuration files, boot parameters
- **Use**: Compiled-in constants
- **Examples**: 
  - Screen resolution: 1920x1080 (hardcoded)
  - GPU PCI ID: hardcoded
  - Memory layout: hardcoded

## Code Size Targets

Aim for **minimal code size**:

- **Bootloader**: < 10 KB
- **Kernel**: < 100 KB
- **GPU Driver**: < 50 KB (hardest part)
- **Input Drivers**: < 20 KB
- **QT6 + App**: ~10-20 MB (unavoidable - QT is big)

**Total OS (without QT)**: ~200 KB or less!

## Performance Targets

We don't care about performance, we care about **working**:

- Boot time: Don't care (< 30 seconds is fine)
- Frame rate: Don't care (30 FPS is plenty)
- Memory usage: Don't care (512 MB is plenty)
- CPU usage: Don't care (can peg one core)

**Trade complexity for simplicity every time.**

## Implementation Strategy

### Phase 1 ✅ (Done)
- Project structure
- Documentation

### Phase 2 (Bootloader)
- UEFI console output (for debugging)
- Load kernel blob
- Jump to kernel
- **Skip**: Complex file systems, just load from known location

### Phase 3 (Kernel)
- Set up page tables (minimal)
- Set up interrupts (only what we need)
- Memory allocator (simple bump allocator)
- **Skip**: Complex memory management

### Phase 4 (GPU)
- PCI scan for RX 6600
- Enable BAR
- Initialize display (hardcode 1920x1080)
- Framebuffer blit
- **Skip**: Everything else GPU can do

### Phase 5 (Input)
- Try PS/2 first (simpler!)
- If not available, minimal USB XHCI
- Keyboard scancodes -> QT key events
- Mouse packets -> QT mouse events
- **Skip**: Complex HID parsing, other devices

### Phase 6 (QT6)
- Port minimal QT6 (Core + Gui + Widgets only)
- QPA plugin: just framebuffer blit + input events
- Static link everything
- **Skip**: All optional QT modules

### Phase 7 (Integration)
- Link app with QT6
- Embed in boot image
- Test and debug
- **Skip**: Anything that doesn't work - iterate

## When in Doubt

Ask these questions:

1. **Does Hello World need this?** No → Cut it
2. **Can we hardcode this?** Yes → Hardcode it
3. **Can we do this simpler?** Yes → Do it simpler
4. **Does Linux do it complex?** Yes → Do it simpler anyway

## Bottom Line

**Build the smallest possible OS that boots QT6 Hello World.**

If it doesn't directly contribute to that goal, it doesn't exist.

No compromises. Maximum minimalism.
