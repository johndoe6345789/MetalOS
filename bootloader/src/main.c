/*
 * MetalOS UEFI Bootloader
 * 
 * MINIMAL bootloader:
 * 1. Get framebuffer from UEFI
 * 2. Load kernel blob from disk
 * 3. Exit boot services
 * 4. Jump to kernel
 * 
 * That's it. No fancy stuff.
 */

#include <stddef.h>
#include "bootloader.h"
#include "efi.h"

// Simplified UEFI System Table structures (bare minimum)
// In a real implementation, we'd use gnu-efi or full UEFI headers

typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    // Simplified - real implementation would have all console I/O functions
    void* pad[10];
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16* FirmwareVendor;
    uint32_t FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    void* ConIn;
    EFI_HANDLE ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
    EFI_HANDLE StandardErrorHandle;
    void* StdErr;
    void* RuntimeServices;
    void* BootServices;
    UINTN NumberOfTableEntries;
    void* ConfigurationTable;
} EFI_SYSTEM_TABLE;

// Global system table
static EFI_SYSTEM_TABLE* gST = NULL;

/*
 * Print a string to the UEFI console
 */
void print_string(const CHAR16* str) {
    (void)str;
    if (gST && gST->ConOut) {
        // In real implementation: gST->ConOut->OutputString(gST->ConOut, (CHAR16*)str);
        // For now, this is a stub
    }
}

/*
 * Print operation status
 */
void print_status(const CHAR16* operation, EFI_STATUS status) {
    // Stub for status reporting
    (void)operation;
    (void)status;
}

/*
 * Initialize graphics output protocol
 */
EFI_STATUS initialize_graphics(EFI_HANDLE ImageHandle) {
    (void)ImageHandle;
    
    // TODO: Implement graphics initialization
    // 1. Locate Graphics Output Protocol
    // 2. Query available modes
    // 3. Select appropriate resolution (prefer 1920x1080 or 1280x720)
    // 4. Set mode
    // 5. Clear screen
    
    return EFI_SUCCESS;
}

/*
 * Load kernel from disk
 */
EFI_STATUS load_kernel(EFI_HANDLE ImageHandle) {
    (void)ImageHandle;
    
    // TODO: Implement kernel loading
    // 1. Open volume protocol
    // 2. Open kernel file (metalos.bin)
    // 3. Read kernel into memory at KERNEL_LOAD_ADDRESS
    // 4. Verify kernel signature/checksum
    
    return EFI_SUCCESS;
}

/*
 * Get ACPI RSDP (Root System Description Pointer)
 */
void* get_rsdp(void) {
    // TODO: Search configuration tables for ACPI RSDP
    return NULL;
}

/*
 * Main entry point for bootloader
 */
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS status;
    BootInfo boot_info = {0};
    (void)boot_info;
    
    gST = SystemTable;
    
    // Print banner
    print_string(u"MetalOS v0.1 - MINIMAL BOOTLOADER\r\n");
    print_string(u"==================================\r\n\r\n");
    
    // Get framebuffer (don't care about resolution, take what UEFI gives us)
    print_string(u"Getting framebuffer...\r\n");
    status = initialize_graphics(ImageHandle);
    if (status != EFI_SUCCESS) {
        print_string(u"WARNING: No graphics, continuing anyway...\r\n");
    }
    
    // Load kernel (just read metalos.bin, don't overthink it)
    print_string(u"Loading kernel...\r\n");
    status = load_kernel(ImageHandle);
    if (status != EFI_SUCCESS) {
        print_string(u"ERROR: Can't load kernel\r\n");
        return status;
    }
    
    // Get memory map (minimal info)
    print_string(u"Getting memory map...\r\n");
    // TODO: GetMemoryMap
    
    // Exit boot services (point of no return)
    print_string(u"Exiting UEFI boot services...\r\n");
    // TODO: ExitBootServices
    
    // Jump to kernel
    // TODO: ((void(*)(BootInfo*))KERNEL_LOAD_ADDRESS)(&boot_info);
    
    // Should never get here
    return EFI_SUCCESS;
}
