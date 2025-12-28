# MetalOS Main Makefile
# Builds bootloader, kernel, and creates bootable image

.PHONY: all bootloader kernel image qemu qemu-debug qemu-gdb clean distclean

all: bootloader kernel

bootloader:
	@echo "Building bootloader..."
	@cd bootloader && $(MAKE)

kernel:
	@echo "Building kernel..."
	@cd kernel && $(MAKE)

# Create bootable disk image
image: bootloader kernel
	@echo "Creating bootable image..."
	@mkdir -p build/iso/EFI/BOOT
	@cp bootloader/bootx64.efi build/iso/EFI/BOOT/
	@cp kernel/metalos.bin build/iso/
	@# TODO: Use xorriso or similar to create proper disk image
	@echo "Image creation requires additional tools (xorriso, mtools)"
	@echo "Manual steps:"
	@echo "  1. Format USB/disk with GPT and FAT32 EFI System Partition"
	@echo "  2. Copy bootloader/bootx64.efi to /EFI/BOOT/ on ESP"
	@echo "  3. Copy kernel/metalos.bin to root of ESP"

# Run in QEMU
qemu: image
	@echo "Starting QEMU..."
	@# Requires OVMF UEFI firmware
	qemu-system-x86_64 \
		-bios /usr/share/ovmf/OVMF.fd \
		-drive format=raw,file=build/metalos.img \
		-m 512M \
		-serial stdio

qemu-debug: image
	@echo "Starting QEMU with debug output..."
	qemu-system-x86_64 \
		-bios /usr/share/ovmf/OVMF.fd \
		-drive format=raw,file=build/metalos.img \
		-m 512M \
		-serial stdio \
		-d int,cpu_reset

qemu-gdb: image
	@echo "Starting QEMU with GDB server on port 1234..."
	qemu-system-x86_64 \
		-bios /usr/share/ovmf/OVMF.fd \
		-drive format=raw,file=build/metalos.img \
		-m 512M \
		-serial stdio \
		-s -S

clean:
	@echo "Cleaning build artifacts..."
	@cd bootloader && $(MAKE) clean
	@cd kernel && $(MAKE) clean
	@rm -rf build

distclean: clean
	@echo "Deep clean..."
	@find . -name "*.o" -delete
	@find . -name "*.d" -delete
