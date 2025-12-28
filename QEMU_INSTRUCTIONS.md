# MetalOS - QEMU Testing Instructions

This document provides instructions for testing MetalOS in QEMU on Windows, macOS, and Linux.

## What You Need

MetalOS is distributed as a bootable disk image (`metalos.img`) that can be run in QEMU with UEFI firmware support. This allows you to test the operating system without installing it on physical hardware.

## Prerequisites

You'll need to install:
1. **QEMU** - A virtual machine emulator
2. **OVMF/EDK2** - UEFI firmware for QEMU (Linux/macOS) or included with QEMU on Windows

---

## Windows Instructions

### Installing QEMU on Windows

1. **Download QEMU for Windows**:
   - Visit https://qemu.weilnetz.de/w64/
   - Download the latest installer (e.g., `qemu-w64-setup-*.exe`)
   - Run the installer and follow the installation wizard
   - Default installation path: `C:\Program Files\qemu`

2. **Add QEMU to PATH** (optional but recommended):
   - Right-click "This PC" → Properties → Advanced system settings
   - Click "Environment Variables"
   - Under "System variables", find "Path" and click "Edit"
   - Click "New" and add: `C:\Program Files\qemu`
   - Click "OK" to save

### Running MetalOS in QEMU (Windows)

**Option 1 - Using Command Prompt (cmd):**

```cmd
cd path\to\metalos\release
"C:\Program Files\qemu\qemu-system-x86_64.exe" ^
  -bios "C:\Program Files\qemu\share\edk2-x86_64-code.fd" ^
  -drive format=raw,file=metalos.img ^
  -m 512M ^
  -serial stdio
```

**Option 2 - Using PowerShell:**

```powershell
cd path\to\metalos\release
& "C:\Program Files\qemu\qemu-system-x86_64.exe" `
  -bios "C:\Program Files\qemu\share\edk2-x86_64-code.fd" `
  -drive format=raw,file=metalos.img `
  -m 512M `
  -serial stdio
```

**Option 3 - Using the provided batch script:**

Create a file named `run_metalos.bat` in the same directory as `metalos.img`:

```batch
@echo off
echo Starting MetalOS in QEMU...
"C:\Program Files\qemu\qemu-system-x86_64.exe" ^
  -bios "C:\Program Files\qemu\share\edk2-x86_64-code.fd" ^
  -drive format=raw,file=metalos.img ^
  -m 512M ^
  -serial stdio
pause
```

Then double-click `run_metalos.bat` to run MetalOS.

### Troubleshooting (Windows)

- **"qemu-system-x86_64 is not recognized"**: Make sure QEMU is installed and added to PATH, or use the full path to the executable.
- **"Could not open BIOS"**: The EDK2 firmware files come with QEMU on Windows. Check that the file exists at `C:\Program Files\qemu\share\edk2-x86_64-code.fd`.
- **Window closes immediately**: Add `pause` at the end of your batch script to see any error messages.

---

## macOS Instructions

### Installing QEMU on macOS

**Using Homebrew** (recommended):

```bash
# Install Homebrew if you haven't already
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install QEMU
brew install qemu
```

**Manual installation**:
- Download QEMU from https://www.qemu.org/download/#macos
- Follow the installation instructions

### Installing OVMF (UEFI Firmware) on macOS

OVMF firmware may need to be downloaded separately:

```bash
# Option 1: Download from your package manager (if available)
brew install qemu  # Usually includes firmware

# Option 2: Download manually from EDK2 project
# Visit: https://github.com/tianocore/edk2/releases
# Download OVMF-X64 package and extract to a known location
```

### Running MetalOS in QEMU (macOS)

**Using Terminal:**

```bash
cd /path/to/metalos/release

# If OVMF firmware is included with QEMU
qemu-system-x86_64 \
  -bios /usr/local/share/qemu/edk2-x86_64-code.fd \
  -drive format=raw,file=metalos.img \
  -m 512M \
  -serial stdio

# If you downloaded OVMF manually
qemu-system-x86_64 \
  -drive if=pflash,format=raw,readonly=on,file=/path/to/OVMF_CODE.fd \
  -drive format=raw,file=metalos.img \
  -m 512M \
  -serial stdio
```

**Create a shell script for easy launching:**

Create a file named `run_metalos.sh`:

```bash
#!/bin/bash
echo "Starting MetalOS in QEMU..."
qemu-system-x86_64 \
  -bios /usr/local/share/qemu/edk2-x86_64-code.fd \
  -drive format=raw,file=metalos.img \
  -m 512M \
  -serial stdio
```

Make it executable and run:

```bash
chmod +x run_metalos.sh
./run_metalos.sh
```

### Troubleshooting (macOS)

- **"qemu-system-x86_64: command not found"**: Make sure QEMU is installed via Homebrew or the binary is in your PATH.
- **"Could not open firmware"**: Check the path to the OVMF firmware file. Common locations:
  - `/usr/local/share/qemu/edk2-x86_64-code.fd`
  - `/opt/homebrew/share/qemu/edk2-x86_64-code.fd` (Apple Silicon Macs)
- **Permission denied**: Run `chmod +x` on your shell script.

---

## Linux Instructions

### Installing QEMU on Linux

**Ubuntu/Debian:**

```bash
sudo apt-get update
sudo apt-get install qemu-system-x86 ovmf
```

**Fedora:**

```bash
sudo dnf install qemu-system-x86 edk2-ovmf
```

**Arch Linux:**

```bash
sudo pacman -S qemu-full edk2-ovmf
```

**Other distributions:**
- Use your distribution's package manager to install `qemu-system-x86` and `ovmf` or `edk2-ovmf`

### Running MetalOS in QEMU (Linux)

**Basic command:**

```bash
cd /path/to/metalos/release

qemu-system-x86_64 \
  -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd \
  -drive format=raw,file=metalos.img \
  -m 512M \
  -serial stdio
```

**With graphical display (GTK):**

```bash
qemu-system-x86_64 \
  -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd \
  -drive format=raw,file=metalos.img \
  -m 512M \
  -serial stdio \
  -display gtk
```

**Create a shell script for easy launching:**

Create a file named `run_metalos.sh`:

```bash
#!/bin/bash
echo "Starting MetalOS in QEMU..."

# Auto-detect OVMF firmware location
if [ -f /usr/share/OVMF/OVMF_CODE.fd ]; then
    OVMF="/usr/share/OVMF/OVMF_CODE.fd"
elif [ -f /usr/share/ovmf/OVMF.fd ]; then
    OVMF="/usr/share/ovmf/OVMF.fd"
elif [ -f /usr/share/edk2-ovmf/x64/OVMF_CODE.fd ]; then
    OVMF="/usr/share/edk2-ovmf/x64/OVMF_CODE.fd"
elif [ -f /usr/share/qemu/ovmf-x86_64.bin ]; then
    OVMF="/usr/share/qemu/ovmf-x86_64.bin"
else
    echo "Error: OVMF firmware not found!"
    echo "Install with: sudo apt-get install ovmf (Ubuntu/Debian)"
    echo "           or: sudo pacman -S edk2-ovmf (Arch)"
    echo "           or: sudo dnf install edk2-ovmf (Fedora)"
    exit 1
fi

echo "Using OVMF firmware: $OVMF"

qemu-system-x86_64 \
  -drive if=pflash,format=raw,readonly=on,file="$OVMF" \
  -drive format=raw,file=metalos.img \
  -m 512M \
  -serial stdio \
  -display gtk
```

Make it executable and run:

```bash
chmod +x run_metalos.sh
./run_metalos.sh
```

### Troubleshooting (Linux)

- **"qemu-system-x86_64: command not found"**: Install QEMU using your package manager.
- **"Could not open firmware"**: Install OVMF/EDK2 firmware package for your distribution.
- **"gtk initialization failed"**: You're in a headless environment. Remove `-display gtk` or use `-display none` or `-nographic`.
- **Permission issues**: Make sure you have read permissions for the firmware files and `metalos.img`.

---

## QEMU Command Line Options Explained

- **`-drive if=pflash,format=raw,readonly=on,file=OVMF_CODE.fd`**: Loads the UEFI firmware
- **`-bios edk2-x86_64-code.fd`**: Alternative way to load UEFI firmware (used on Windows)
- **`-drive format=raw,file=metalos.img`**: Loads the MetalOS disk image as a raw disk
- **`-m 512M`**: Allocates 512MB of RAM to the virtual machine
- **`-serial stdio`**: Redirects serial output to the terminal (useful for debugging)
- **`-display gtk`**: Uses GTK for the graphical window (Linux)
- **`-display none`**: Runs in headless mode (no graphics)
- **`-nographic`**: Disables graphical output entirely

---

## Advanced Usage

### Running with more memory:

```bash
# Allocate 1GB of RAM instead of 512MB
qemu-system-x86_64 ... -m 1024M ...
```

### Enable KVM acceleration (Linux only):

```bash
# Requires KVM kernel module
qemu-system-x86_64 ... -enable-kvm ...
```

### Debug mode:

```bash
# Enable debug output
qemu-system-x86_64 ... -d int,cpu_reset ...
```

### GDB debugging:

```bash
# Start QEMU with GDB server on port 1234
qemu-system-x86_64 ... -s -S ...

# In another terminal:
gdb
(gdb) target remote localhost:1234
(gdb) continue
```

---

## Expected Behavior

MetalOS is currently in early development. Depending on the development phase:

- **Phase 1-2**: May boot to UEFI shell (this is normal during early development)
- **Phase 3+**: Should display kernel boot messages via serial output
- **Phase 7+**: Will eventually boot directly to a QT6 application

Check the serial output (terminal/console) for boot messages and debug information.

---

## Getting Help

If you encounter issues:

1. Check the troubleshooting section for your operating system above
2. Verify QEMU and OVMF/EDK2 are properly installed
3. Check the serial output for error messages
4. Visit the MetalOS repository: https://github.com/johndoe6345789/MetalOS

---

## System Requirements

- **CPU**: x86_64 (64-bit) processor
- **RAM**: Minimum 512MB free memory for the virtual machine
- **Disk**: ~100MB for QEMU and MetalOS files
- **OS**: Windows 10+, macOS 10.13+, or modern Linux distribution

---

## Additional Resources

- QEMU Documentation: https://www.qemu.org/docs/master/
- OVMF/EDK2 Project: https://github.com/tianocore/edk2
- MetalOS Repository: https://github.com/johndoe6345789/MetalOS
