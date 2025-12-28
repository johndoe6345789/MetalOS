# MetalOS Dependencies

This directory contains third-party dependencies managed in-house for MetalOS development.

## Directory Structure

```
deps/
├── firmware/          # AMD GPU firmware blobs
├── mesa-radv/         # Mesa RADV Vulkan driver source
├── qt6/               # QT6 framework (minimal build)
└── ovmf/              # UEFI firmware for testing
```

## Firmware Blobs

**Location**: `deps/firmware/`

AMD Radeon RX 6600 (Navi 23) requires several firmware files:
- `dimgrey_cavefish_ce.bin` - Command Engine firmware
- `dimgrey_cavefish_me.bin` - MicroEngine firmware
- `dimgrey_cavefish_mec.bin` - MEC (Compute) firmware
- `dimgrey_cavefish_pfp.bin` - Pre-Fetch Parser firmware
- `dimgrey_cavefish_rlc.bin` - Run List Controller firmware
- `dimgrey_cavefish_sdma.bin` - SDMA engine firmware
- `dimgrey_cavefish_vcn.bin` - Video Core Next firmware

### Obtaining Firmware

These firmware files are available from the Linux firmware repository:
```bash
# Clone linux-firmware repository
git clone https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git /tmp/linux-firmware

# Copy Navi 23 firmware files
cp /tmp/linux-firmware/amdgpu/dimgrey_cavefish_*.bin deps/firmware/
```

**License**: These files are redistributable under the AMD GPU firmware license (see firmware files for details).

## Mesa RADV

**Location**: `deps/mesa-radv/`

Mesa RADV is the userspace Vulkan driver for AMD GPUs. MetalOS uses RADV to avoid implementing a Vulkan driver from scratch.

### Setup

```bash
# Clone Mesa repository (or download specific release)
git clone https://gitlab.freedesktop.org/mesa/mesa.git /tmp/mesa
cd /tmp/mesa

# Checkout stable version (e.g., 24.0)
git checkout mesa-24.0.0

# Copy RADV driver sources to deps
cp -r src/amd/vulkan ../MetalOS/deps/mesa-radv/
```

We will compile RADV against our custom `libradv-metal` bridge instead of Linux's libdrm.

**License**: MIT License (see Mesa source for full license text)

## QT6 Framework

**Location**: `deps/qt6/`

QT6 is the application framework used by the single MetalOS application.

### Setup

```bash
# Download QT6 source (minimal modules only)
# We only need QtCore, QtGui, and QtWidgets
wget https://download.qt.io/official_releases/qt/6.5/6.5.3/single/qt-everywhere-src-6.5.3.tar.xz

# Extract and configure minimal build
tar xf qt-everywhere-src-6.5.3.tar.xz
cd qt-everywhere-src-6.5.3

# Configure for MetalOS (minimal, static linking)
./configure -static -no-dbus -no-ssl -no-cups -no-fontconfig \
            -prefix ../MetalOS/deps/qt6
```

**Note**: Adjust the `-prefix` path to match your MetalOS repository location. Using a relative path from the extracted QT source directory.

**License**: LGPL v3 / GPL v2 / Commercial (see QT documentation)

## OVMF (EDK II)

**Location**: `deps/ovmf/`

OVMF provides UEFI firmware for QEMU testing.

### Setup

```bash
# Download pre-built OVMF binaries
mkdir -p deps/ovmf
wget https://www.kraxel.org/repos/jenkins/edk2/edk2.git-ovmf-x64-0-20230524.209.gf0064ac3af.EOL.no.nore.updates.noarch.rpm

# Extract OVMF files
rpm2cpio edk2.git-ovmf-*.rpm | cpio -idmv
cp usr/share/edk2/ovmf/OVMF_CODE.fd deps/ovmf/
cp usr/share/edk2/ovmf/OVMF_VARS.fd deps/ovmf/
```

Alternatively, build from source:
```bash
git clone https://github.com/tianocore/edk2.git
cd edk2
git submodule update --init
make -C BaseTools
source edksetup.sh
build -a X64 -t GCC5 -p OvmfPkg/OvmfPkgX64.dsc
```

**License**: BSD-2-Clause (see EDK II license)

## Version Management

Each dependency should be tracked with a specific version/commit:

- **Firmware**: Linux firmware commit hash
- **Mesa RADV**: Mesa version tag (e.g., `mesa-24.0.0`)
- **QT6**: QT version (e.g., `6.5.3`)
- **OVMF**: EDK II commit hash or release tag

Create a `VERSION` file in each subdirectory documenting the exact version in use.

## Build Integration

The main Makefile will be updated to:
1. Check for dependencies in `deps/`
2. Use in-house versions when available
3. Fall back to system versions if needed
4. Provide targets to download/build dependencies

## Rationale

Managing dependencies in-house provides:
- **Reproducibility**: Exact versions are locked and available
- **Offline development**: No external downloads required
- **Control**: We can patch or customize dependencies as needed
- **Simplicity**: Single repository contains everything needed to build

## Size Considerations

The deps directory may become large (~500MB-1GB). Consider:
- Using `.gitattributes` for Git LFS on large binary files
- Documenting exact download/build steps instead of committing binaries
- Selective inclusion based on development phase

For now, we document the structure and process. Actual population of dependencies will occur as needed during implementation phases.
