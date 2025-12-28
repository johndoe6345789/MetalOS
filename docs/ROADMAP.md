# MetalOS Roadmap

## Vision

MetalOS is a **minimal operating system** built from the ground up with a single purpose: run a QT6 Hello World application full-screen on AMD64 hardware with a Radeon RX 6600 GPU, booting via UEFI.

This is not a general-purpose OS. Every component is purpose-built to achieve this specific goal with minimal complexity.

## Development Phases

### Phase 1: Foundation ✓ (Current)

**Goal**: Establish project structure and documentation

- [x] Create project directory structure
- [x] Write architecture documentation
- [x] Define build system
- [x] Create bootloader skeleton
- [x] Create kernel skeleton
- [x] Create QT6 hello world application template
- [x] Document development workflow

**Deliverables**:
- Project structure in place
- Documentation framework
- Skeleton code for bootloader and kernel
- Build system (Makefiles)

### Phase 2: UEFI Bootloader (Next)

**Goal**: Boot from UEFI and load kernel

**Tasks**:
1. Implement UEFI protocol interfaces
   - Console I/O for early debugging
   - Graphics Output Protocol
   - Simple File System Protocol
   - Memory allocation

2. Graphics initialization
   - Query available video modes
   - Set optimal resolution (1920x1080 or best available)
   - Set up framebuffer

3. Kernel loading
   - Read metalos.bin from disk
   - Load into memory at 1MB mark
   - Verify kernel integrity

4. System information gathering
   - Get memory map
   - Find ACPI tables (RSDP)
   - Detect CPU features

5. Exit boot services and jump to kernel
   - Call ExitBootServices()
   - Pass BootInfo structure to kernel
   - Jump to kernel_main()

**Success Criteria**: Bootloader loads kernel and jumps to kernel code

### Phase 3: Minimal Kernel

**Goal**: Initialize hardware and provide basic services

**Tasks**:
1. Early kernel initialization
   - Set up GDT (Global Descriptor Table)
   - Set up IDT (Interrupt Descriptor Table)
   - Enable interrupts
   - Initialize framebuffer console

2. Memory management
   - Physical memory allocator (buddy system or bitmap)
   - Virtual memory setup (page tables)
   - Kernel heap allocator
   - *Minimal implementation - just enough for QT6*

3. Process/Thread support
   - Simple round-robin scheduler
   - Context switching (bare minimum)
   - Single user process support
   - *No multi-user, no fancy scheduling*

4. Basic I/O
   - Serial port for debugging
   - Framebuffer console
   - *No disk I/O needed initially*

**Success Criteria**: Kernel boots, prints messages, can allocate memory

### Phase 4: Hardware Abstraction Layer

**Goal**: Support minimal hardware needed for QT6

**Tasks**:
1. PCI Bus enumeration
   - Scan PCI devices
   - Find Radeon RX 6600 GPU
   - Basic configuration

2. GPU Driver (Radeon RX 6600)
   - Initialize GPU
   - Set up display pipeline
   - Configure framebuffer
   - *Minimal - no 3D acceleration initially*
   - *Can use reference from Linux amdgpu driver*

3. Input devices
   - USB HID keyboard support
   - USB HID mouse support
   - PS/2 fallback (if needed)
   - *Just enough for QT event handling*

4. Timer
   - APIC timer or PIT
   - For scheduling and timeouts

**Success Criteria**: Can detect and initialize GPU, receive keyboard/mouse input

### Phase 5: System Call Interface

**Goal**: Provide user-kernel boundary

**Tasks**:
1. System call mechanism
   - syscall/sysret instructions
   - System call table
   - Parameter passing

2. Essential system calls
   - exit() - terminate process
   - write() - output to console/log
   - mmap() - memory allocation
   - open/read/close() - minimal file operations (if needed)
   - ioctl() - device control (for GPU)
   - poll/select() - event handling (for input)

3. User-kernel transitions
   - Ring 3 to Ring 0 transitions
   - Parameter validation
   - Error handling

**Success Criteria**: User space can make system calls

### Phase 6: User Space Runtime

**Goal**: Support C++ applications

**Tasks**:
1. ELF loader
   - Parse ELF headers
   - Load program segments
   - Set up entry point
   - *Static linking initially - no dynamic loader*

2. Minimal C/C++ runtime
   - _start() function
   - C++ global constructors/destructors
   - Memory allocation (malloc/free)
   - Basic string functions
   - *Only what QT6 needs*

3. Application launcher (no init/shell needed)
   - Directly load QT6 hello world application
   - No command line, no shell
   - Single application until reboot

**Success Criteria**: Can load and run the QT6 hello world application directly

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

- Phase 1: 1 week ✓ (Complete)
- Phase 2: 2-3 weeks
- Phase 3: 3-4 weeks
- Phase 4: 4-6 weeks
- Phase 5: 1-2 weeks
- Phase 6: 2-3 weeks
- Phase 7: 6-8 weeks
- Phase 8: 2-3 weeks

**Total**: ~4-6 months of focused development

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

5. **SMP**: ❌ Not needed
   - Single core is fine for hello world

6. **ACPI**: ⚠️ Minimal
   - Just enough to work with GPU

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
