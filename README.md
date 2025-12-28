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


1) Reality check: where the bloat really lives (RDNA2)

On Navi 23, you will not get good performance without:
	•	GPU firmware blobs (various dimgrey_cavefish_*.bin files; Navi 23’s codename is “dimgrey cavefish”, and Linux systems load firmware files with that prefix).  ￼
	•	A real memory manager (VRAM/GTT, page tables, buffer objects)
	•	Command submission (rings/queues) + fences/semaphores
	•	A Vulkan driver implementation (or reuse one)

So the “least bloat” strategy is: reuse a Vulkan implementation (Mesa RADV is the obvious candidate), but avoid importing a whole Unix stack by giving it a very small kernel/userspace interface tailored to your OS.

RADV is explicitly a userspace Vulkan driver for modern AMD GPUs.  ￼

⸻

2) The best “toy OS but fast” plan: RADV + a tiny amdgpu-shaped shim

Why this is the sweet spot
	•	You keep your OS non-POSIX.
	•	You avoid writing a Vulkan driver from scratch (the truly hard part).
	•	You implement only the kernel-facing parts RADV needs: a buffer object + VM + submit + sync API.

Shape of the stack

MetalOS kernel
	•	PCIe enumeration, BAR mapping
	•	interrupts (MSI/MSI-X)
	•	DMA mapping (or identity-map if you’re being reckless)
	•	a GPU kernel driver that exposes a small ioctl-like API

Userspace
	•	gpu-service (optional but recommended for structure)
	•	libradv-metal (a minimal libdrm-like bridge)
	•	Mesa RADV compiled against your bridge (not Linux libdrm)

This is “Unix-like internally” only in the sense of interfaces, not user experience.

⸻

3) Minimal kernel GPU API (the smallest set that still performs)

Think in terms of four pillars:

A) Firmware load + ASIC init
	•	gpu_load_firmware(name, blob)
	•	gpu_init() → returns chip info (gfx1032, VRAM size, doorbells, etc.)

You will need those Navi23 firmware blobs (again: dimgrey_cavefish_*.bin family is the practical breadcrumb).  ￼

B) Buffer objects (BOs)
	•	bo_create(size, domain=VRAM|GTT, flags)
	•	bo_map(bo) / bo_unmap(bo) (CPU mapping)
	•	bo_export_handle(bo) (so Vulkan can bind memory)

C) Virtual memory (GPU page tables)
	•	vm_create()
	•	vm_map(vm, bo, gpu_va, size, perms)
	•	vm_unmap(vm, gpu_va, size)

D) Submission + synchronization
	•	queue_create(type=GFX|COMPUTE|DMA)
	•	queue_submit(queue, cs_buffer, fence_out)
	•	fence_wait(fence, timeout)
	•	timeline_semaphore_* (optional, but hugely useful)

If you implement these correctly, you get real GPU throughput.

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
make qemu     # Test in QEMU with UEFI firmware
make clean    # Clean build artifacts
```

**QEMU UEFI Testing**:
```bash
make qemu                      # Boot in QEMU with UEFI (headless)
make qemu QEMU_DISPLAY=gtk     # Boot with graphical display
make qemu-debug                # Boot with debug output
make qemu-gdb                  # Boot with GDB debugging
make qemu-uefi-test            # Test UEFI firmware setup
```

See [docs/BUILD.md](docs/BUILD.md) for detailed build instructions and [docs/TESTING.md](docs/TESTING.md) for testing guide.

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
- **QEMU (UEFI MODE)**: For testing the OS.

## License

See [LICENSE](LICENSE) file for details.
