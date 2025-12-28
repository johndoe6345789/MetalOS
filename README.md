# MetalOS
A OS that is not linux, not bsd and not windows. Ground up OS with just enough bespoke kernel to run a QT6 hello world full screen app on AMD64 with Radeon RX 6600 GPU., UEFI native

- Might use Linux kernel only as inspiration for driver code, can be git cloned.
- Some changes might be required to QT6 - We can git clone this.
- Mostly ignoring other peoples OS work.
- One OS launches this One Full Screen QT6 app. No command line, no cruft.

Key Design Decisions:

✅ No command line/shell - boots directly to QT6 app
✅ No file system - app embedded in boot image
✅ Single application only
✅ Static linking for simplicity
✅ Minimal implementation philosophy
✅ Creative freedom for OS components, precision for drivers

Phase 1: Project Foundation ✅ COMPLETE

Review project requirements

Create project structure and documentation

Define architecture (minimal, single-app boot)

Create UEFI bootloader skeleton

Set up build system (Makefiles)

Create basic kernel entry point with framebuffer console

Document development environment setup

Create QT6 Hello World application template

Clarify design: no shell, boot directly to app

Add comprehensive status documentation

Phase 2: UEFI Bootloader (Next)

Implement UEFI protocol interfaces

Graphics initialization (GOP)

Kernel loading from disk

Memory map retrieval

Exit boot services

Jump to kernel with boot info
Phase 3: Core Kernel Components

GDT/IDT setup

Interrupt handling

Memory management (physical/virtual)

Simple scheduler (single process)

Basic framebuffer console improvements
Phase 4: Hardware Support

PCI device enumeration

Radeon RX 6600 GPU driver (precise implementation)

Framebuffer management

USB input (keyboard/mouse)
Phase 5: System Call Interface

Minimal syscall mechanism

Essential syscalls for QT6

User/kernel mode transitions
Phase 6: User Space & Application

ELF loader (static only)

C++ runtime support

Load QT6 hello world directly (no init)

Static-linked application
Phase 7: QT6 Port

Identify and port QT6 dependencies

Create MetalOS QPA (Platform Abstraction) plugin

Framebuffer graphics backend

Input event integration

Build QT6 for MetalOS
Phase 8: Integration & Testing

Build complete bootable image

Full-screen rendering validation

Philosophy: If it doesn't help QT6 Hello World, it doesn't exist.


What We Cut (see docs/MINIMALISM.md):

❌ Scheduler (one app = always running)
❌ Process management (one process only)
❌ File system (app embedded)
❌ Networking (not needed)
❌ Shell/CLI (boot directly to app)
❌ Security (trust everything)
❌ Multi-core (one CPU)
❌ Dynamic linking (static only)
❌ Virtual memory complexity (identity map)
❌ ACPI (try without it)
What We Keep (absolute minimum):

✅ Memory allocator (bump allocator)
✅ ~5 interrupt handlers (timer, keyboard, mouse, GPU)
✅ GPU driver (framebuffer only, ~50 KB)
✅ Input drivers (PS/2 first, USB fallback, ~20 KB)
✅ PCI scan (just find our GPU)
✅ ~5 syscalls (write, mmap, ioctl, poll, exit)

Input handling verification

Test on QEMU

Test on real hardware (AMD64 + RX 6600)
