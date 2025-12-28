# MetalOS Roadmap

## Vision

MetalOS is **the absolute minimum operating system** needed to boot QT6 Hello World on AMD64 + RX 6600 via UEFI.

**Target**: < 200 KB of OS code (excluding QT6)

**Philosophy**: If it doesn't directly enable QT6 Hello World, it doesn't exist.

## Development Phases

### Phase 1: Foundation ✅ COMPLETE

**Goal**: Establish project structure and documentation

- [x] Create project directory structure
- [x] Write architecture documentation
- [x] Define build system
- [x] Create bootloader skeleton
- [x] Create kernel skeleton
- [x] Create QT6 hello world application template
- [x] Document development workflow

**Deliverables**:
- ✅ Project structure in place
- ✅ Documentation framework
- ✅ Skeleton code for bootloader and kernel
- ✅ Build system (Makefiles, CMake, Conan)

### Phase 2: UEFI Bootloader ✅ COMPLETE

**Goal**: Load kernel and jump (< 10 KB code)

**Tasks**:
- [x] Console output for debugging (UEFI OutputString)
- [x] Get framebuffer info from GOP (Graphics Output Protocol)
- [x] Load kernel blob from known location
- [x] Get minimal memory map
- [x] Exit boot services
- [x] Jump to kernel
- [x] ACPI RSDP discovery (bonus)
- [x] Multiple build system support (bonus)

**Achievements**:
- ✅ Complete UEFI bootloader implementation (~6.3 KB)
- ✅ Valid PE32+ EFI executable
- ✅ Comprehensive UEFI protocol definitions
- ✅ Support for Make, CMake, and Conan build systems
- ✅ Full documentation and testing

**Success Criteria**: ✅ Bootloader successfully loads kernel and provides boot information

### Phase 3: Minimal Kernel (In Progress)

**Goal**: Initialize hardware (< 100 KB code)

**Tasks**:
1. Minimal paging setup
   - Identity map or simple offset
   - Just enough to keep CPU happy
   - No fancy page fault handling

2. Interrupt handling
   - [x] IDT with handlers
   - [x] Timer interrupt
   - [ ] Keyboard interrupt
   - [ ] Mouse interrupt
   - [x] Basic interrupt framework

3. Memory allocator
   - [x] Basic memory management structures
   - [ ] Bump allocator or simple free list
   - [ ] malloc/free for applications

4. Core Components
   - [x] GDT (Global Descriptor Table)
   - [x] IDT (Interrupt Descriptor Table)
   - [x] APIC (Advanced Programmable Interrupt Controller)
   - [x] PCI scanning
   - [x] SMP/multicore support framework
   - [x] Spinlocks for synchronization

**Progress**:
- ✅ Core kernel structure in place
- ✅ GDT and IDT setup
- ✅ Basic interrupt handling
- ✅ APIC initialization
- ✅ PCI device enumeration
- ✅ SMP/multicore support framework
- ⏳ Memory allocator needs completion
- ⏳ Device drivers need implementation

**Success Criteria**: Kernel boots, prints messages, mallocs work

### Phase 4: Hardware Support

**Goal**: GPU + Input (< 70 KB code)

**Tasks**:
1. PCI scan (minimal)
   - Hardcode scan for vendor 0x1002 (AMD)
   - Find RX 6600 device ID
   - Enable memory access
   - That's it!

2. GPU Driver (hardest part, < 50 KB)
   - Map MMIO registers
   - Initialize display controller (DCN)
   - Hardcode 1920x1080 mode
   - Set up framebuffer in VRAM
   - Enable display output
   - *Study Linux amdgpu driver, but keep it simple*

3. Input devices (< 20 KB)
   - **Try PS/2 first** (much simpler than USB!)
   - If no PS/2: minimal USB XHCI for HID
   - Keyboard: scancode → QT key events
   - Mouse: packets → QT mouse events

**Simplifications**:
- No PCI tree traversal, just scan for our GPU
- No GPU 3D, just 2D framebuffer
- No hot-plug, no multiple displays
- PS/2 over USB if possible (way simpler)
- No timer sophistication, PIT is fine

**Success Criteria**: Display working, keyboard/mouse input working

### Phase 5: Minimal Syscalls

**Goal**: User-kernel boundary (< 10 KB code)

**Tasks**:
1. Syscall mechanism (syscall/sysret or just direct calls)
2. Maybe 5-10 syscalls total:
   - write() - debugging
   - mmap() / munmap() - memory
   - ioctl() - device control
   - poll() - wait for input events
   - exit() - halt system

**Simplifications**:
- No syscall table if app can directly call kernel functions
- No parameter validation (trust everything)
- No error handling (just panic)

**Success Criteria**: App can call kernel functions

### Phase 6: User Space & Application

**Goal**: Load and run app

**Tasks**:
1. Simple app loading
   - Static-linked binary
   - No ELF parsing if we can avoid it
   - Just jump to known entry point

2. C++ minimal runtime
   - Global constructors
   - Basic new/delete (use kernel malloc)
   - No exceptions (disable them)

3. Direct boot to app
   - No init, no shell
   - Kernel calls app's main() directly
   - App exits → kernel halts

**Simplifications**:
- Static link everything into one blob
- Skip ELF loader completely if possible
- No exceptions (compile with -fno-exceptions)
- App runs in ring 0 or simple ring 3

**Success Criteria**: Can run simple C++ program

### Phase 7: QT6 Port

**Goal**: Port QT6 to MetalOS

**Tasks**:
1. QT6 dependencies
   - Port minimal C++ standard library
   - Port required libraries (zlib, png, freetype, etc.)
   - *Only what QT6 absolutely needs*

2. QT Platform Abstraction (QPA) Plugin
   - MetalOS platform plugin
   - Framebuffer graphics backend
   - Input event integration
   - Event loop integration with kernel

3. Build QT6 for MetalOS
   - Configure QT6 build system
   - Cross-compile QT6
   - Strip unnecessary modules
   - *QtCore, QtGui, QtWidgets only*

4. Test infrastructure
   - Simple QT apps for testing
   - Verify widgets render
   - Verify input works

**Success Criteria**: Simple QT6 applications can run

### Phase 8: Integration & Polish

**Goal**: Get Hello World running perfectly

**Tasks**:
1. Build hello world application
   - Compile against MetalOS QT6
   - Static link everything
   - Create bootable image

2. Full-screen rendering
   - Ensure application fills screen
   - Proper resolution handling
   - Clean graphics output

3. Input handling
   - Keyboard input works
   - Mouse input works (if used by app)
   - Clean event handling

4. Performance tuning
   - Optimize critical paths
   - Reduce boot time
   - Smooth rendering

5. Testing
   - Test on QEMU
   - Test on real hardware (AMD64 + RX 6600)
   - Fix any hardware-specific issues

**Success Criteria**: QT6 Hello World runs full-screen on target hardware

## Timeline Estimates

*These are rough estimates for a single developer*

- Phase 1: 1 week ✅ (Complete)
- Phase 2: 2-3 weeks ✅ (Complete)
- Phase 3: 3-4 weeks (In Progress)
- Phase 4: 4-6 weeks
- Phase 5: 1-2 weeks
- Phase 6: 2-3 weeks
- Phase 7: 6-8 weeks
- Phase 8: 2-3 weeks

**Total**: ~4-6 months of focused development

**Status**: Phase 2 complete, Phase 3 in progress with core kernel components implemented (GDT, IDT, interrupts, memory management, SMP support, PCI scanning).

## Technical Challenges

### Major Challenges

1. **GPU Initialization**: Radeon RX 6600 is a modern GPU with complex initialization. May need to study Linux amdgpu driver extensively.

2. **QT6 Dependencies**: QT6 has many dependencies. Need to port or stub out non-essential ones.

3. **QPA Plugin**: Creating a QT Platform Abstraction plugin from scratch is non-trivial.

4. **Memory Management**: Need working virtual memory before user space.

5. **USB Stack**: If using USB input, need minimal USB stack (XHCI for modern systems).

### Risk Mitigation

- Start with serial/VGA console before GPU
- Use PS/2 input before USB if easier
- Test incrementally in QEMU before hardware
- Study existing minimal OS implementations
- Reference Linux kernel code (but don't copy)

## Success Metrics

### Minimal Success
- Boots on target hardware
- Displays something via GPU
- Can be interacted with via keyboard
- QT6 hello world shows text

### Full Success
- Boots reliably on AMD64 + RX 6600
- Full screen 1920x1080 rendering
- Clean QT6 widget rendering
- Responsive input handling
- Boot time < 10 seconds

## Resources Needed

### Hardware
- AMD64 system with Radeon RX 6600 GPU
- USB keyboard and mouse
- Serial port (optional, for debugging)

### Software
- Cross-compiler toolchain (x86_64-elf-gcc)
- QEMU with UEFI support
- GDB for debugging
- QT6 source code

### Knowledge
- UEFI specification
- x86_64 architecture
- PCI/PCIe protocol
- AMD GPU documentation
- QT6 architecture
- OS development fundamentals

## Design Decisions

1. **File System**: ❌ Not needed
   - Application embedded in boot image or loaded by bootloader
   - Everything in RAM

2. **Command Line / Shell**: ❌ Not needed
   - Boot directly into QT6 application
   - No init process, no shell

3. **Networking**: ❌ Not needed
   - Hello world doesn't need network

4. **Dynamic Linking**: ❌ Not needed
   - Static link everything for simplicity

5. **SMP**: ⚠️ Basic support implemented
   - Basic multicore/SMP framework in place for learning
   - Single core is sufficient for hello world
   - Can be simplified later if needed

6. **ACPI**: ✅ Implemented
   - RSDP discovery in bootloader
   - Needed for hardware discovery

7. **Multiple Applications**: ❌ Not needed
   - One application only - the QT6 hello world

## Contributing

This is a learning/demonstration project. Contributions welcome, but:
- Keep it minimal
- Every feature must justify its existence
- Ask "Does this help run QT6 hello world?" before adding anything
- Prefer simple over clever
- Document everything

## References

- [UEFI Specification](https://uefi.org/specifications)
- [OSDev Wiki](https://wiki.osdev.org/)
- [AMD64 Manual](https://www.amd.com/en/support/tech-docs)
- [Linux amdgpu driver](https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/amd)
- [QT6 Source](https://code.qt.io/cgit/qt/qt5.git/)
- [QT QPA Documentation](https://doc.qt.io/qt-6/qpa.html)
