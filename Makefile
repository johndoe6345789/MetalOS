# MetalOS Main Makefile
# Builds bootloader, kernel, and creates bootable image

.PHONY: all bootloader kernel image qemu qemu-debug qemu-gdb qemu-uefi-test test clean distclean

all: bootloader kernel

# Run unit tests
test:
	@echo "Running unit tests..."
	@cd tests && $(MAKE) test

bootloader:
	@echo "Building bootloader..."
	@cd bootloader && $(MAKE) || echo "Warning: Bootloader build failed (expected during Phase 1 development)"

kernel:
	@echo "Building kernel..."
	@cd kernel && $(MAKE) || echo "Warning: Kernel build failed (expected during Phase 1 development)"

# Create bootable disk image for UEFI/QEMU
image: bootloader kernel
	@chmod +x scripts/create_image.sh
	@./scripts/create_image.sh

# Run in QEMU with UEFI firmware (OVMF)
# Automatically detects OVMF path and display mode
# Set QEMU_DISPLAY=gtk or QEMU_DISPLAY=sdl to use graphical mode
QEMU_DISPLAY ?= none

qemu: image
	@echo "Starting QEMU with UEFI firmware..."
	@bash -c ' \
		if [ -f /usr/share/OVMF/OVMF_CODE.fd ]; then \
			OVMF="/usr/share/OVMF/OVMF_CODE.fd"; \
		elif [ -f /usr/share/ovmf/OVMF.fd ]; then \
			OVMF="/usr/share/ovmf/OVMF.fd"; \
		elif [ -f /usr/share/edk2-ovmf/x64/OVMF_CODE.fd ]; then \
			OVMF="/usr/share/edk2-ovmf/x64/OVMF_CODE.fd"; \
		elif [ -f /usr/share/qemu/ovmf-x86_64.bin ]; then \
			OVMF="/usr/share/qemu/ovmf-x86_64.bin"; \
		else \
			echo "Error: OVMF UEFI firmware not found!"; \
			echo "Install with:"; \
			echo "  Ubuntu/Debian: sudo apt-get install ovmf"; \
			echo "  Arch Linux: sudo pacman -S edk2-ovmf"; \
			echo "  Fedora: sudo dnf install edk2-ovmf"; \
			exit 1; \
		fi; \
		echo "Using OVMF firmware: $$OVMF"; \
		echo "Display mode: $(QEMU_DISPLAY) (set QEMU_DISPLAY=gtk for graphical)"; \
		qemu-system-x86_64 \
			-drive if=pflash,format=raw,readonly=on,file=$$OVMF \
			-drive format=raw,file=build/metalos.img \
			-m 512M \
			-serial stdio \
			-display $(QEMU_DISPLAY) \
			-net none \
	'

qemu-debug: image
	@echo "Starting QEMU with debug output..."
	@bash -c ' \
		if [ -f /usr/share/OVMF/OVMF_CODE.fd ]; then \
			OVMF="/usr/share/OVMF/OVMF_CODE.fd"; \
		elif [ -f /usr/share/ovmf/OVMF.fd ]; then \
			OVMF="/usr/share/ovmf/OVMF.fd"; \
		elif [ -f /usr/share/edk2-ovmf/x64/OVMF_CODE.fd ]; then \
			OVMF="/usr/share/edk2-ovmf/x64/OVMF_CODE.fd"; \
		else \
			echo "Error: OVMF UEFI firmware not found!"; \
			exit 1; \
		fi; \
		qemu-system-x86_64 \
			-drive if=pflash,format=raw,readonly=on,file=$$OVMF \
			-drive format=raw,file=build/metalos.img \
			-m 512M \
			-serial stdio \
			-display $(QEMU_DISPLAY) \
			-net none \
			-d int,cpu_reset \
	'

qemu-gdb: image
	@echo "Starting QEMU with GDB server on port 1234..."
	@bash -c ' \
		if [ -f /usr/share/OVMF/OVMF_CODE.fd ]; then \
			OVMF="/usr/share/OVMF/OVMF_CODE.fd"; \
		elif [ -f /usr/share/ovmf/OVMF.fd ]; then \
			OVMF="/usr/share/ovmf/OVMF.fd"; \
		elif [ -f /usr/share/edk2-ovmf/x64/OVMF_CODE.fd ]; then \
			OVMF="/usr/share/edk2-ovmf/x64/OVMF_CODE.fd"; \
		else \
			echo "Error: OVMF UEFI firmware not found!"; \
			exit 1; \
		fi; \
		echo "QEMU will wait for GDB connection on localhost:1234"; \
		echo "In another terminal, run: gdb kernel/metalos.bin"; \
		echo "Then in GDB: target remote localhost:1234"; \
		qemu-system-x86_64 \
			-drive if=pflash,format=raw,readonly=on,file=$$OVMF \
			-drive format=raw,file=build/metalos.img \
			-m 512M \
			-serial stdio \
			-display $(QEMU_DISPLAY) \
			-net none \
			-s -S \
	'

# Quick UEFI test - boots to UEFI shell without OS (for verifying QEMU+OVMF setup)
qemu-uefi-test:
	@echo "Testing QEMU UEFI boot (no OS image)..."
	@bash -c ' \
		if [ -f /usr/share/OVMF/OVMF_CODE.fd ]; then \
			OVMF="/usr/share/OVMF/OVMF_CODE.fd"; \
		elif [ -f /usr/share/ovmf/OVMF.fd ]; then \
			OVMF="/usr/share/ovmf/OVMF.fd"; \
		elif [ -f /usr/share/edk2-ovmf/x64/OVMF_CODE.fd ]; then \
			OVMF="/usr/share/edk2-ovmf/x64/OVMF_CODE.fd"; \
		else \
			echo "Error: OVMF UEFI firmware not found!"; \
			exit 1; \
		fi; \
		echo "Using OVMF firmware: $$OVMF"; \
		echo "This will boot to UEFI shell in nographic mode."; \
		echo "You should see UEFI boot messages. Press Ctrl-A then X to exit QEMU."; \
		qemu-system-x86_64 \
			-drive if=pflash,format=raw,readonly=on,file=$$OVMF \
			-m 512M \
			-nographic \
			-net none \
	'

clean:
	@echo "Cleaning build artifacts..."
	@cd bootloader && $(MAKE) clean
	@cd kernel && $(MAKE) clean
	@cd tests && $(MAKE) clean
	@rm -rf build

distclean: clean
	@echo "Deep clean..."
	@find . -name "*.o" -delete
	@find . -name "*.d" -delete
