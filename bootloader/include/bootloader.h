#ifndef METALOS_BOOTLOADER_BOOTLOADER_H
#define METALOS_BOOTLOADER_BOOTLOADER_H

#include "efi.h"

// Bootloader version
#define BOOTLOADER_VERSION_MAJOR 0
#define BOOTLOADER_VERSION_MINOR 1
#define BOOTLOADER_VERSION_PATCH 0

// Memory limits
#define KERNEL_LOAD_ADDRESS 0x100000  // 1MB mark
#define MAX_KERNEL_SIZE     0x1000000 // 16MB max

// Function declarations
EFI_STATUS initialize_graphics(EFI_HANDLE ImageHandle, BootInfo* boot_info);
EFI_STATUS load_kernel(EFI_HANDLE ImageHandle, BootInfo* boot_info);
void* get_rsdp(void);
void print_string(const CHAR16* str);
void print_status(const CHAR16* operation, EFI_STATUS status);

#endif // METALOS_BOOTLOADER_BOOTLOADER_H
