# GPU Implementation Strategy

## Overview

This document outlines the GPU implementation strategy for MetalOS targeting the AMD Radeon RX 6600 (RDNA2 / Navi 23 architecture).

## Reality Check: Where the Bloat Really Lives (RDNA2)

On Navi 23, you will not get good performance without:
- GPU firmware blobs (various `dimgrey_cavefish_*.bin` files; Navi 23's codename is "dimgrey cavefish", and Linux systems load firmware files with that prefix)
- A real memory manager (VRAM/GTT, page tables, buffer objects)
- Command submission (rings/queues) + fences/semaphores
- A Vulkan driver implementation (or reuse one)

So the "least bloat" strategy is: reuse a Vulkan implementation (Mesa RADV is the obvious candidate), but avoid importing a whole Unix stack by giving it a very small kernel/userspace interface tailored to your OS.

RADV is explicitly a userspace Vulkan driver for modern AMD GPUs.

---

## The Best "Toy OS but Fast" Plan: RADV + a Tiny amdgpu-shaped Shim

### Why This is the Sweet Spot

- You keep your OS non-POSIX
- You avoid writing a Vulkan driver from scratch (the truly hard part)
- You implement only the kernel-facing parts RADV needs: a buffer object + VM + submit + sync API

### Shape of the Stack

**MetalOS Kernel:**
- PCIe enumeration, BAR mapping
- Interrupts (MSI/MSI-X)
- DMA mapping (or identity-map if you're being reckless)
- A GPU kernel driver that exposes a small ioctl-like API

**Userspace:**
- `gpu-service` (optional but recommended for structure)
- `libradv-metal` (a minimal libdrm-like bridge)
- Mesa RADV compiled against your bridge (not Linux libdrm)

This is "Unix-like internally" only in the sense of interfaces, not user experience.

---

## Minimal Kernel GPU API (The Smallest Set That Still Performs)

Think in terms of four pillars:

### A) Firmware Load + ASIC Init

```c
gpu_load_firmware(name, blob)
gpu_init() → returns chip info (gfx1032, VRAM size, doorbells, etc.)
```

You will need those Navi23 firmware blobs (again: `dimgrey_cavefish_*.bin` family is the practical breadcrumb).

### B) Buffer Objects (BOs)

```c
bo_create(size, domain=VRAM|GTT, flags)
bo_map(bo) / bo_unmap(bo)           // CPU mapping
bo_export_handle(bo)                 // so Vulkan can bind memory
```

### C) Virtual Memory (GPU Page Tables)

```c
vm_create()
vm_map(vm, bo, gpu_va, size, perms)
vm_unmap(vm, gpu_va, size)
```

### D) Submission + Synchronization

```c
queue_create(type=GFX|COMPUTE|DMA)
queue_submit(queue, cs_buffer, fence_out)
fence_wait(fence, timeout)
timeline_semaphore_*                 // optional, but hugely useful
```

If you implement these correctly, you get real GPU throughput.

---

## Implementation Notes

- Focus on the minimal API surface that RADV requires
- Firmware blobs are non-negotiable for Navi 23 performance
- Memory management (VRAM/GTT) is critical for proper GPU operation
- Command submission infrastructure must be solid for reliability
- Synchronization primitives (fences/semaphores) enable proper GPU-CPU coordination

## References

- Mesa RADV driver source code
- AMD GPU specifications for RDNA2 architecture
- Linux amdgpu kernel driver for reference implementation patterns
