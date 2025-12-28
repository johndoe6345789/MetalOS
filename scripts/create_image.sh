#!/bin/bash
# MetalOS - Create bootable disk image for UEFI/QEMU

set -e

echo "MetalOS Image Creator"
echo "====================="

BUILD_DIR="build"
ISO_DIR="$BUILD_DIR/iso"
IMAGE="$BUILD_DIR/metalos.img"

# Check if bootloader and kernel exist
if [ ! -f "bootloader/bootx64.efi" ]; then
    echo "Warning: bootloader/bootx64.efi not found."
    echo "Creating placeholder bootloader for testing..."
    mkdir -p bootloader
    # NOTE: This creates a text placeholder, NOT a valid EFI executable.
    # UEFI firmware will fail to execute it, but the image structure will be correct.
    # This is intentional during early development (Phase 1).
    # In Phase 2+, a proper UEFI bootloader will be built.
    echo "Placeholder UEFI bootloader - not executable" > bootloader/bootx64.efi
fi

if [ ! -f "kernel/metalos.bin" ]; then
    echo "Warning: kernel/metalos.bin not found."
    echo "Creating placeholder kernel for testing..."
    mkdir -p kernel
    # NOTE: This creates a text placeholder, NOT a valid kernel binary.
    # This is intentional during early development (Phase 1).
    # In Phase 3+, a proper kernel binary will be built.
    echo "Placeholder kernel - not executable" > kernel/metalos.bin
fi

# Create directories
mkdir -p "$ISO_DIR/EFI/BOOT"

# Copy files
echo "Copying bootloader..."
cp bootloader/bootx64.efi "$ISO_DIR/EFI/BOOT/"

echo "Copying kernel..."
cp kernel/metalos.bin "$ISO_DIR/"

# Create disk image using different methods based on available tools
echo "Creating disk image..."

# Method 1: Try mtools (preferred for QEMU UEFI boot)
if command -v mformat &> /dev/null && command -v mcopy &> /dev/null; then
    echo "Using mtools to create FAT32 disk image..."
    
    # Create a 64MB disk image with FAT32
    dd if=/dev/zero of="$IMAGE" bs=1M count=64 2>/dev/null
    
    # Format as FAT32
    mformat -i "$IMAGE" -F -v "METALOS" ::
    
    # Create EFI directory structure
    mmd -i "$IMAGE" ::/EFI
    mmd -i "$IMAGE" ::/EFI/BOOT
    
    # Copy bootloader and kernel
    mcopy -i "$IMAGE" bootloader/bootx64.efi ::/EFI/BOOT/
    mcopy -i "$IMAGE" kernel/metalos.bin ::/
    
    echo "Success! Created $IMAGE using mtools"
    
# Method 2: Try xorriso (ISO9660 with El Torito)
elif command -v xorriso &> /dev/null; then
    echo "Using xorriso to create ISO image..."
    xorriso -as mkisofs \
        -e EFI/BOOT/bootx64.efi \
        -no-emul-boot \
        -o "$IMAGE" \
        "$ISO_DIR"
    echo "Success! Created $IMAGE using xorriso"
    
# Method 3: Fallback - create simple FAT image with dd and manual copy instructions
else
    echo "Warning: Neither mtools nor xorriso found."
    echo "Creating basic disk image, but it may not be bootable."
    echo ""
    
    # Create a basic image
    dd if=/dev/zero of="$IMAGE" bs=1M count=64 2>/dev/null
    
    echo "Files are ready in $ISO_DIR/"
    echo ""
    echo "To make this image bootable, install mtools:"
    echo "  Ubuntu/Debian: sudo apt-get install mtools"
    echo "  Arch Linux: sudo pacman -S mtools"
    echo "  macOS: brew install mtools"
    echo ""
    echo "Then run this script again, or manually create a bootable USB:"
    echo "  1. Format USB with GPT and FAT32 EFI System Partition"
    echo "  2. Copy $ISO_DIR/EFI to the USB"
    echo "  3. Copy $ISO_DIR/metalos.bin to the USB"
fi

echo ""
echo "Image ready for QEMU UEFI testing!"
echo "Run: make qemu"
