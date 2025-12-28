#!/bin/bash
# MetalOS Dependency Setup Script
# Downloads and sets up all required dependencies for building MetalOS

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
METALOS_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
DEPS_DIR="$METALOS_ROOT/deps"

echo "=== MetalOS Dependency Setup ==="
echo "Root directory: $METALOS_ROOT"
echo "Dependencies directory: $DEPS_DIR"
echo ""

# Create deps directories if they don't exist
mkdir -p "$DEPS_DIR"/{firmware,ovmf,mesa-radv,qt6}

# Function to download AMD GPU firmware blobs
setup_firmware() {
    echo "=== Setting up AMD GPU Firmware Blobs ==="
    
    FIRMWARE_DIR="$DEPS_DIR/firmware"
    TEMP_DIR=$(mktemp -d)
    
    cd "$TEMP_DIR"
    
    # Clone linux-firmware repository (shallow clone for speed)
    echo "Downloading linux-firmware repository..."
    git clone --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git
    
    # Copy required Navi 23 (dimgrey_cavefish) firmware files
    echo "Copying Navi 23 firmware files..."
    FIRMWARE_FILES=(
        "dimgrey_cavefish_ce.bin"
        "dimgrey_cavefish_me.bin"
        "dimgrey_cavefish_mec.bin"
        "dimgrey_cavefish_pfp.bin"
        "dimgrey_cavefish_rlc.bin"
        "dimgrey_cavefish_sdma.bin"
        "dimgrey_cavefish_vcn.bin"
    )
    
    for file in "${FIRMWARE_FILES[@]}"; do
        if [ -f "linux-firmware/amdgpu/$file" ]; then
            cp "linux-firmware/amdgpu/$file" "$FIRMWARE_DIR/"
            echo "  ✓ Copied $file"
        else
            echo "  ⚠ Warning: $file not found"
        fi
    done
    
    # Get commit hash for version tracking
    cd linux-firmware
    COMMIT_HASH=$(git rev-parse HEAD)
    COMMIT_DATE=$(git log -1 --format=%cd --date=short)
    
    # Create VERSION file
    cat > "$FIRMWARE_DIR/VERSION" <<EOF
Linux Firmware Repository
Commit: $COMMIT_HASH
Date: $COMMIT_DATE
Source: https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git

Files:
EOF
    
    for file in "${FIRMWARE_FILES[@]}"; do
        if [ -f "$FIRMWARE_DIR/$file" ]; then
            SHA256=$(sha256sum "$FIRMWARE_DIR/$file" | cut -d' ' -f1)
            echo "  $file (SHA256: $SHA256)" >> "$FIRMWARE_DIR/VERSION"
        fi
    done
    
    cd "$METALOS_ROOT"
    rm -rf "$TEMP_DIR"
    
    echo "✓ Firmware blobs setup complete"
    echo ""
}

# Function to setup OVMF UEFI firmware
setup_ovmf() {
    echo "=== Setting up OVMF UEFI Firmware ==="
    
    OVMF_DIR="$DEPS_DIR/ovmf"
    
    # Check for OVMF in system locations
    if [ -f /usr/share/OVMF/OVMF_CODE.fd ]; then
        echo "Copying OVMF from /usr/share/OVMF/..."
        cp /usr/share/OVMF/OVMF_CODE.fd "$OVMF_DIR/" 2>/dev/null || true
        cp /usr/share/OVMF/OVMF_VARS.fd "$OVMF_DIR/" 2>/dev/null || true
    elif [ -f /usr/share/ovmf/OVMF.fd ]; then
        echo "Copying OVMF from /usr/share/ovmf/..."
        cp /usr/share/ovmf/OVMF.fd "$OVMF_DIR/OVMF_CODE.fd" 2>/dev/null || true
    elif [ -f /usr/share/edk2-ovmf/x64/OVMF_CODE.fd ]; then
        echo "Copying OVMF from /usr/share/edk2-ovmf/..."
        cp /usr/share/edk2-ovmf/x64/OVMF_CODE.fd "$OVMF_DIR/" 2>/dev/null || true
        cp /usr/share/edk2-ovmf/x64/OVMF_VARS.fd "$OVMF_DIR/" 2>/dev/null || true
    elif [ -f /usr/share/qemu/ovmf-x86_64.bin ]; then
        echo "Copying OVMF from /usr/share/qemu/..."
        cp /usr/share/qemu/ovmf-x86_64.bin "$OVMF_DIR/OVMF_CODE.fd" 2>/dev/null || true
    else
        echo "⚠ Warning: OVMF firmware not found in system paths"
        echo "  Install with: sudo apt-get install ovmf (Ubuntu/Debian)"
        echo "               sudo pacman -S edk2-ovmf (Arch Linux)"
    fi
    
    # Create VERSION file
    if [ -f "$OVMF_DIR/OVMF_CODE.fd" ]; then
        cat > "$OVMF_DIR/VERSION" <<EOF
OVMF UEFI Firmware
Source: System installation
Date: $(date +%Y-%m-%d)

Files:
EOF
        for file in "$OVMF_DIR"/*.fd; do
            if [ -f "$file" ]; then
                FILENAME=$(basename "$file")
                SHA256=$(sha256sum "$file" | cut -d' ' -f1)
                echo "  $FILENAME (SHA256: $SHA256)" >> "$OVMF_DIR/VERSION"
            fi
        done
        echo "✓ OVMF firmware setup complete"
    else
        echo "⚠ OVMF setup incomplete - no firmware files copied"
    fi
    
    echo ""
}

# Function to setup Mesa RADV (placeholder for now)
setup_mesa_radv() {
    echo "=== Setting up Mesa RADV Driver ==="
    
    MESA_DIR="$DEPS_DIR/mesa-radv"
    
    cat > "$MESA_DIR/STATUS" <<EOF
Mesa RADV Driver Setup

Status: Planned for Phase 4 (Hardware Support)

The Mesa RADV Vulkan driver will be integrated to provide GPU support
for the AMD Radeon RX 6600 without implementing a full Vulkan driver
from scratch.

Planned approach:
1. Clone Mesa repository
2. Extract RADV driver sources
3. Build against custom libradv-metal bridge (instead of libdrm)
4. Integrate with MetalOS kernel GPU API

For manual setup, see deps/mesa-radv/README.md
EOF
    
    echo "ℹ Mesa RADV setup deferred to Phase 4"
    echo "  See $MESA_DIR/STATUS for details"
    echo ""
}

# Function to setup QT6 (placeholder for now)
setup_qt6() {
    echo "=== Setting up QT6 Framework ==="
    
    QT6_DIR="$DEPS_DIR/qt6"
    
    cat > "$QT6_DIR/STATUS" <<EOF
QT6 Framework Setup

Status: Planned for Phase 7 (QT6 Port)

QT6 will be compiled as a minimal static build with only the modules
needed for the single MetalOS application.

Required modules:
- QtCore
- QtGui
- QtWidgets

Planned approach:
1. Download QT6 source (6.5.x series)
2. Configure minimal static build
3. Build against MetalOS platform plugin
4. Strip unnecessary features

For manual setup, see deps/qt6/README.md
EOF
    
    echo "ℹ QT6 setup deferred to Phase 7"
    echo "  See $QT6_DIR/STATUS for details"
    echo ""
}

# Main execution
main() {
    case "${1:-all}" in
        firmware)
            setup_firmware
            ;;
        ovmf)
            setup_ovmf
            ;;
        mesa)
            setup_mesa_radv
            ;;
        qt6)
            setup_qt6
            ;;
        all)
            setup_firmware
            setup_ovmf
            setup_mesa_radv
            setup_qt6
            ;;
        *)
            echo "Usage: $0 [firmware|ovmf|mesa|qt6|all]"
            echo ""
            echo "Options:"
            echo "  firmware - Download AMD GPU firmware blobs"
            echo "  ovmf     - Setup OVMF UEFI firmware"
            echo "  mesa     - Setup Mesa RADV driver (placeholder)"
            echo "  qt6      - Setup QT6 framework (placeholder)"
            echo "  all      - Setup all dependencies (default)"
            exit 1
            ;;
    esac
    
    echo "=== Dependency Setup Complete ==="
}

main "$@"
