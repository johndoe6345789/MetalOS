#!/bin/bash
# MetalOS - Create bootable disk image

set -e

echo "MetalOS Image Creator"
echo "====================="

BUILD_DIR="build"
ISO_DIR="$BUILD_DIR/iso"
IMAGE="$BUILD_DIR/metalos.img"

# Check if bootloader and kernel exist
if [ ! -f "bootloader/bootx64.efi" ]; then
    echo "Error: bootloader/bootx64.efi not found. Run 'make bootloader' first."
    exit 1
fi

if [ ! -f "kernel/metalos.bin" ]; then
    echo "Error: kernel/metalos.bin not found. Run 'make kernel' first."
    exit 1
fi

# Create directories
mkdir -p "$ISO_DIR/EFI/BOOT"

# Copy files
echo "Copying bootloader..."
cp bootloader/bootx64.efi "$ISO_DIR/EFI/BOOT/"

echo "Copying kernel..."
cp kernel/metalos.bin "$ISO_DIR/"

# Create disk image (requires mtools and xorriso)
echo "Creating disk image..."

if command -v xorriso &> /dev/null; then
    xorriso -as mkisofs \
        -e EFI/BOOT/bootx64.efi \
        -no-emul-boot \
        -o "$IMAGE" \
        "$ISO_DIR"
    echo "Success! Created $IMAGE"
else
    echo "Warning: xorriso not found. Image not created."
    echo "Install with: sudo apt-get install xorriso"
    echo ""
    echo "Files are ready in $ISO_DIR/"
    echo "You can manually create a bootable USB by:"
    echo "  1. Format USB with GPT and FAT32 EFI partition"
    echo "  2. Copy $ISO_DIR/EFI to the USB"
    echo "  3. Copy $ISO_DIR/metalos.bin to the USB"
fi
