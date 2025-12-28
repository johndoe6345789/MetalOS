# Mesa RADV Driver

This directory will contain the Mesa RADV (AMD Vulkan driver) source code.

## Overview

RADV is a userspace Vulkan driver for AMD GPUs. MetalOS uses RADV to provide Vulkan support without implementing a driver from scratch.

## Source

Mesa RADV is part of the [Mesa 3D Graphics Library](https://gitlab.freedesktop.org/mesa/mesa).

## Setup Instructions

```bash
# Clone Mesa repository
git clone https://gitlab.freedesktop.org/mesa/mesa.git /tmp/mesa
cd /tmp/mesa

# Checkout stable version
git checkout mesa-24.0.0

# Copy RADV sources
cp -r src/amd/vulkan/* /path/to/MetalOS/deps/mesa-radv/
```

## Integration with MetalOS

MetalOS will compile RADV against a custom `libradv-metal` bridge that provides:
- Buffer object management
- Virtual memory (GPU page tables)
- Command submission
- Synchronization primitives

This replaces the standard Linux libdrm dependency.

## License

MIT License - See Mesa source for full license text.

## Version Tracking

Create a `VERSION` file documenting:
- Mesa version/tag (e.g., `mesa-24.0.0`)
- Git commit hash
- Date obtained
- Any local modifications

## Status

⚠️ **Not yet populated** - RADV sources will be added during Phase 7 (QT6 Port)
