# QEMU UEFI Testing Guide

This guide covers testing MetalOS in QEMU with UEFI firmware (OVMF).

## Overview

MetalOS uses QEMU with OVMF (Open Virtual Machine Firmware) to test UEFI boot functionality without requiring physical hardware. This enables rapid development and testing of the bootloader and kernel.

## Prerequisites

### Required Software

1. **QEMU** (version 6.0 or later recommended)
   - Provides x86_64 virtualization
   - Supports UEFI firmware

2. **OVMF** (UEFI firmware for QEMU)
   - Open-source UEFI implementation
   - Required for UEFI boot in QEMU

3. **mtools** (for disk image creation)
   - Creates FAT32 filesystems
   - Manipulates MS-DOS filesystems

### Installation

**Ubuntu/Debian**:
```bash
sudo apt-get update
sudo apt-get install qemu-system-x86 ovmf mtools xorriso
```

**Arch Linux**:
```bash
sudo pacman -S qemu-full edk2-ovmf mtools xorriso
```

**Fedora**:
```bash
sudo dnf install qemu-system-x86 edk2-ovmf mtools xorriso
```

**macOS**:
```bash
brew install qemu mtools xorriso
# Note: OVMF may need manual installation
```

## Quick Start

### Basic QEMU Boot

```bash
# Build and boot MetalOS in QEMU
make qemu
```

This command:
1. Builds the bootloader and kernel (or uses placeholders)
2. Creates a FAT32 disk image with UEFI boot structure
3. Launches QEMU with OVMF firmware
4. Boots the system in headless mode

**Expected Output**:
```
Building bootloader...
Building kernel...
Creating bootable image...
Using mtools to create FAT32 disk image...
Success! Created build/metalos.img
Starting QEMU with UEFI firmware...
Using OVMF firmware: /usr/share/ovmf/OVMF.fd
Display mode: none (set QEMU_DISPLAY=gtk for graphical)
[UEFI boot messages...]
```

### Verify UEFI Setup

To verify that QEMU and OVMF are properly installed:

```bash
make qemu-uefi-test
```

This boots directly to the UEFI shell without any OS image, confirming your setup works.

## Display Modes

MetalOS supports multiple display modes for different use cases:

### Headless Mode (Default)

```bash
make qemu
# or explicitly
make qemu QEMU_DISPLAY=none
```

**Use cases**:
- CI/CD pipelines
- Headless servers
- Automated testing
- Serial console debugging

**Characteristics**:
- No graphical window
- Serial output only
- Works in any environment

### Graphical Mode - GTK

```bash
make qemu QEMU_DISPLAY=gtk
```

**Use cases**:
- Interactive development
- Visual debugging
- UI testing

**Characteristics**:
- Native GTK window
- Full graphics support
- Requires X11/Wayland

### Graphical Mode - SDL

```bash
make qemu QEMU_DISPLAY=sdl
```

**Use cases**:
- Cross-platform development
- Alternative to GTK

**Characteristics**:
- SDL window
- Good compatibility
- Lighter than GTK

### Text Mode - Curses

```bash
make qemu QEMU_DISPLAY=curses
```

**Use cases**:
- SSH sessions
- Text-only environments

**Characteristics**:
- Terminal-based UI
- No X11 required
- Works over SSH

## Advanced Testing

### Debug Mode

Boot with CPU interrupt and reset debugging:

```bash
make qemu-debug
```

**Additional debug output includes**:
- CPU interrupts
- CPU resets
- Exception handling
- Hardware events

### GDB Debugging

Debug the kernel with GDB:

**Terminal 1** - Start QEMU with GDB server:
```bash
make qemu-gdb
```

QEMU will wait for GDB connection on `localhost:1234`.

**Terminal 2** - Connect GDB:
```bash
gdb kernel/metalos.bin
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

**Useful GDB commands**:
```gdb
# Set breakpoints
break *0x100000          # Break at address
break kernel_main        # Break at function

# Examine memory
x/10i $rip              # Disassemble 10 instructions
x/10x 0x100000          # Show 10 hex values

# Step through code
stepi                   # Step one instruction
nexti                   # Step over calls
continue                # Continue execution

# Register inspection
info registers          # Show all registers
print $rax              # Print specific register
```

## Disk Image Details

### Image Structure

The `build/metalos.img` file contains:

```
metalos.img (64 MB FAT32 disk)
├── EFI/
│   └── BOOT/
│       └── bootx64.efi    # UEFI bootloader
└── metalos.bin             # Kernel binary
```

### Inspecting the Image

View the disk image contents:

```bash
# List files in the image
mdir -i build/metalos.img ::/

# List EFI boot directory
mdir -i build/metalos.img ::/EFI/BOOT/

# Extract a file
mcopy -i build/metalos.img ::/metalos.bin extracted_kernel.bin

# Get image info
file build/metalos.img
```

### Manual Image Creation

If you need to manually create or modify the image:

```bash
# Create empty 64MB image
dd if=/dev/zero of=disk.img bs=1M count=64

# Format as FAT32
mformat -i disk.img -F -v "METALOS" ::

# Create directory structure
mmd -i disk.img ::/EFI
mmd -i disk.img ::/EFI/BOOT

# Copy files
mcopy -i disk.img bootloader/bootx64.efi ::/EFI/BOOT/
mcopy -i disk.img kernel/metalos.bin ::/
```

## OVMF Firmware

### Firmware Locations

The Makefile automatically detects OVMF firmware in these locations:

- `/usr/share/OVMF/OVMF_CODE.fd` (Ubuntu/Debian)
- `/usr/share/ovmf/OVMF.fd` (Ubuntu/Debian alternative)
- `/usr/share/edk2-ovmf/x64/OVMF_CODE.fd` (Arch Linux)
- `/usr/share/qemu/ovmf-x86_64.bin` (Other systems)

### Firmware Variants

**OVMF_CODE.fd**: Code-only firmware (read-only)
- Used with `-drive if=pflash,readonly=on`
- Best for testing
- Variables stored separately

**OVMF.fd**: Combined code and variables
- Single file for firmware
- Simpler setup
- Variables reset on each boot

**OVMF.secboot.fd**: Secure Boot enabled
- For secure boot testing
- Requires signed bootloader

## Troubleshooting

### QEMU Won't Start

**Issue**: `gtk initialization failed` or `Could not initialize SDL`

**Solution**: Use headless mode:
```bash
make qemu QEMU_DISPLAY=none
```

### OVMF Not Found

**Issue**: `Error: OVMF UEFI firmware not found!`

**Solution**: Install OVMF package:
```bash
# Ubuntu/Debian
sudo apt-get install ovmf

# Arch Linux
sudo pacman -S edk2-ovmf
```

### Image Creation Fails

**Issue**: `mformat: command not found`

**Solution**: Install mtools:
```bash
# Ubuntu/Debian
sudo apt-get install mtools

# Arch Linux
sudo pacman -S mtools
```

### Boots to UEFI Shell

**Expected Behavior**: During development, booting to UEFI shell is normal until the bootloader is complete.

**What it means**:
- ✅ QEMU and OVMF working correctly
- ✅ Disk image recognized (shows as FS0)
- ⚠️ Bootloader not yet functional (expected in Phase 1)

**Next steps**: Implement Phase 2 (UEFI Bootloader)

### Serial Output Missing

**Issue**: No serial console output

**Solution**: Ensure serial port is configured correctly:
- Output goes to stdio by default
- Check that `-serial stdio` is in QEMU command
- Verify kernel writes to serial port

### QEMU Hangs

**Issue**: QEMU appears to hang during boot

**Possible causes**:
1. Bootloader infinite loop
2. Waiting for input
3. Hardware initialization issue

**Debug steps**:
```bash
# Use debug mode
make qemu-debug

# Use GDB to inspect state
make qemu-gdb
# In another terminal: gdb kernel/metalos.bin
```

## CI/CD Integration

### GitHub Actions

Example workflow for QEMU testing:

```yaml
name: QEMU Boot Test

on: [push, pull_request]

jobs:
  qemu-test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y qemu-system-x86 ovmf mtools
      
      - name: Build and test in QEMU
        run: |
          make qemu QEMU_DISPLAY=none &
          QEMU_PID=$!
          sleep 10
          kill $QEMU_PID || true
```

### Docker Testing

Run QEMU tests in Docker:

```dockerfile
FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential nasm qemu-system-x86 ovmf mtools

WORKDIR /metalos
COPY . .

CMD ["make", "qemu", "QEMU_DISPLAY=none"]
```

## Performance Tuning

### Memory Allocation

Default: 512 MB

Adjust memory:
```bash
# Edit Makefile QEMU command
qemu-system-x86_64 ... -m 1024M  # Use 1GB
```

### CPU Configuration

Use multiple cores:
```bash
qemu-system-x86_64 ... -smp 2  # 2 cores
```

### KVM Acceleration

Enable KVM for faster emulation (Linux only):
```bash
qemu-system-x86_64 ... -enable-kvm
```

**Note**: Requires KVM kernel module and permissions.

## Best Practices

1. **Always test in headless mode first** - Ensures CI/CD compatibility
2. **Use serial output for debugging** - More reliable than graphics
3. **Keep disk images small** - 64 MB is sufficient for development
4. **Test with different OVMF versions** - Ensures compatibility
5. **Document boot behavior changes** - Track progress in commits
6. **Use GDB for serious debugging** - More powerful than printf
7. **Test on multiple systems** - Catch compatibility issues early

## Resources

- [QEMU Documentation](https://www.qemu.org/docs/master/)
- [OVMF Wiki](https://github.com/tianocore/tianocore.github.io/wiki/OVMF)
- [UEFI Specification](https://uefi.org/specifications)
- [OSDev Wiki - UEFI](https://wiki.osdev.org/UEFI)

## Related Documentation

- [BUILD.md](BUILD.md) - Build system and toolchain
- [TESTING.md](TESTING.md) - Complete testing guide
- [ROADMAP.md](ROADMAP.md) - Development phases
- [DEVELOPMENT.md](DEVELOPMENT.md) - Development workflow
