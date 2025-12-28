# OVMF UEFI Firmware

This directory contains OVMF (Open Virtual Machine Firmware) for QEMU testing.

## Overview

OVMF provides UEFI firmware implementation for QEMU, allowing us to test MetalOS in a UEFI environment.

## Required Files

- `OVMF_CODE.fd` - UEFI firmware code
- `OVMF_VARS.fd` - UEFI variables template

## Source

OVMF is part of the [EDK II project](https://github.com/tianocore/edk2).

## Obtaining OVMF

### Option 1: Use Pre-built Binaries

Many distributions provide OVMF packages:
```bash
# Ubuntu/Debian
sudo apt-get install ovmf

# Copy to deps folder
cp /usr/share/OVMF/OVMF_CODE.fd deps/ovmf/
cp /usr/share/OVMF/OVMF_VARS.fd deps/ovmf/
```

### Option 2: Download Pre-built Binaries

Pre-built OVMF binaries are available from various sources. One option is Gerd Hoffmann's builds:

```bash
# Check https://www.kraxel.org/repos/jenkins/edk2/ for available builds
# Example with a specific build (check for newer versions):
wget https://www.kraxel.org/repos/jenkins/edk2/edk2.git-ovmf-x64-0-20230524.209.gf0064ac3af.EOL.no.nore.updates.noarch.rpm

rpm2cpio edk2.git-ovmf-*.rpm | cpio -idmv
cp usr/share/edk2/ovmf/OVMF_CODE.fd deps/ovmf/
cp usr/share/edk2/ovmf/OVMF_VARS.fd deps/ovmf/
```

**Note**: The specific build URL may change. Visit https://www.kraxel.org/repos/jenkins/edk2/ for current builds or use system packages (Option 1) instead.

### Option 3: Build from Source

```bash
git clone https://github.com/tianocore/edk2.git
cd edk2
git submodule update --init
make -C BaseTools
source edksetup.sh
build -a X64 -t GCC5 -p OvmfPkg/OvmfPkgX64.dsc

# Files will be in Build/OvmfX64/RELEASE_GCC5/FV/
cp Build/OvmfX64/RELEASE_GCC5/FV/OVMF_CODE.fd ../MetalOS/deps/ovmf/
cp Build/OvmfX64/RELEASE_GCC5/FV/OVMF_VARS.fd ../MetalOS/deps/ovmf/
```

## Usage with QEMU

```bash
qemu-system-x86_64 \
    -bios deps/ovmf/OVMF_CODE.fd \
    -drive file=metalos.img,format=raw \
    -m 512M
```

## License

BSD-2-Clause - See EDK II license for full details.

## Version Tracking

Create a `VERSION` file documenting:
- EDK II commit hash or version
- Build date
- Source (pre-built package, jenkins, or self-built)
- File checksums (SHA256)

## Status

⚠️ **Optional** - Can use system-installed OVMF or this in-house copy. The Makefile already detects system OVMF paths.
