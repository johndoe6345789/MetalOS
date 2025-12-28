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

// Global system table
static EFI_SYSTEM_TABLE* gST = NULL;
static EFI_BOOT_SERVICES* gBS = NULL;

/**
 * @brief Compares two UEFI GUIDs for equality
 * 
 * GUIDs (Globally Unique Identifiers) are 128-bit values used by UEFI to identify
 * protocols, tables, and other system resources. This function performs a field-by-field
 * comparison of two GUID structures.
 * 
 * @param a Pointer to the first GUID to compare
 * @param b Pointer to the second GUID to compare
 * @return 1 if the GUIDs are equal, 0 if they differ
 * 
 * @note The GUID structure consists of:
 *       - Data1: 32-bit value
 *       - Data2: 16-bit value
 *       - Data3: 16-bit value
 *       - Data4: 8-byte array
 */
static int guid_compare(const EFI_GUID* a, const EFI_GUID* b) {
    if (a->Data1 != b->Data1) return 0;
    if (a->Data2 != b->Data2) return 0;
    if (a->Data3 != b->Data3) return 0;
    for (int i = 0; i < 8; i++) {
        if (a->Data4[i] != b->Data4[i]) return 0;
    }
    return 1;
}

/**
 * @brief Sets a block of memory to a specified value
 * 
 * This is a simple implementation of memset that works in the UEFI environment.
 * It fills the first n bytes of the memory area pointed to by s with the constant
 * byte c. This is used for initializing structures and buffers.
 * 
 * @param s Pointer to the memory block to fill
 * @param c Value to set (converted to unsigned char)
 * @param n Number of bytes to set
 * @return Pointer to the memory block s
 * 
 * @note This function operates byte-by-byte, so it's not optimized for large blocks.
 *       However, for bootloader usage with small structures, this is sufficient.
 */
static VOID* efi_memset(VOID* s, int c, UINTN n) {
    unsigned char* p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

/**
 * @brief Copies a block of memory from source to destination
 * 
 * This is a simple implementation of memcpy that works in the UEFI environment.
 * It copies n bytes from memory area src to memory area dest. The memory areas
 * must not overlap (use memmove if they might overlap).
 * 
 * @param dest Pointer to the destination memory block
 * @param src Pointer to the source memory block
 * @param n Number of bytes to copy
 * @return Pointer to the destination memory block dest
 * 
 * @warning The memory areas must not overlap. If they do, the behavior is undefined.
 * @note This function operates byte-by-byte for simplicity and UEFI compatibility.
 */
static VOID* efi_memcpy(VOID* dest, const VOID* src, UINTN n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) *d++ = *s++;
    return dest;
}

/**
 * @brief Prints a UTF-16 string to the UEFI console output
 * 
 * Uses the UEFI Simple Text Output Protocol to display a string on the console.
 * This is the primary method for user feedback during the boot process.
 * 
 * @param str Pointer to a null-terminated UTF-16 string to display
 * 
 * @note UEFI uses UTF-16 encoding (CHAR16*) rather than ASCII or UTF-8.
 *       Literal strings should be prefixed with 'u' (e.g., u"Hello").
 * @note This function does nothing if the console output protocol is unavailable,
 *       which may happen in headless systems.
 */
void print_string(const CHAR16* str) {
    if (gST && gST->ConOut) {
        gST->ConOut->OutputString(gST->ConOut, (CHAR16*)str);
    }
}

/**
 * @brief Prints an operation description followed by its status result
 * 
 * This helper function displays the result of a UEFI operation in a consistent
 * format: the operation description followed by either " [OK]" or " [FAILED]"
 * depending on the status code.
 * 
 * @param operation UTF-16 string describing the operation that was performed
 * @param status UEFI status code returned by the operation (EFI_SUCCESS or error)
 * 
 * @note EFI_SUCCESS (0) indicates success; any other value indicates failure.
 * @see print_string() for the underlying output mechanism
 */
void print_status(const CHAR16* operation, EFI_STATUS status) {
    print_string(operation);
    if (status == EFI_SUCCESS) {
        print_string(u" [OK]\r\n");
    } else {
        print_string(u" [FAILED]\r\n");
    }
}

/**
 * @brief Initializes the graphics output and retrieves framebuffer information
 * 
 * This function locates the UEFI Graphics Output Protocol (GOP) and extracts
 * framebuffer details needed by the kernel for direct graphics rendering.
 * The GOP provides a linear framebuffer that can be used for pixel-based graphics.
 * 
 * The function stores the following information in boot_info:
 * - framebuffer_base: Physical address of the framebuffer in memory
 * - framebuffer_width: Horizontal resolution in pixels
 * - framebuffer_height: Vertical resolution in pixels
 * - framebuffer_pitch: Bytes per scanline (width * bytes_per_pixel, may include padding)
 * - framebuffer_bpp: Bits per pixel (assumed 32-bit BGRA format)
 * 
 * @param ImageHandle Handle to the bootloader image (currently unused)
 * @param boot_info Pointer to BootInfo structure to receive framebuffer information
 * @return EFI_SUCCESS if GOP was located and framebuffer info was retrieved,
 *         or an error code if GOP is unavailable
 * 
 * @note This function uses the current graphics mode without attempting to change it.
 *       The resolution is whatever UEFI firmware has already configured.
 * @note The framebuffer format is assumed to be 32-bit (4 bytes per pixel).
 */
EFI_STATUS initialize_graphics(EFI_HANDLE ImageHandle, BootInfo* boot_info) {
    (void)ImageHandle;
    EFI_STATUS status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
    
    // Locate Graphics Output Protocol
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    status = gBS->LocateProtocol(&gop_guid, NULL, (void**)&gop);
    
    if (status != EFI_SUCCESS || gop == NULL) {
        return status;
    }
    
    // Use current mode (don't try to change it - keep it simple)
    if (gop->Mode && gop->Mode->Info) {
        boot_info->framebuffer_base = gop->Mode->FrameBufferBase;
        boot_info->framebuffer_width = gop->Mode->Info->HorizontalResolution;
        boot_info->framebuffer_height = gop->Mode->Info->VerticalResolution;
        boot_info->framebuffer_pitch = gop->Mode->Info->PixelsPerScanLine * 4; // Assume 32-bit
        boot_info->framebuffer_bpp = 32; // Assume 32-bit color
    }
    
    return EFI_SUCCESS;
}

/**
 * @brief Loads the kernel binary from the boot disk into memory
 * 
 * This function performs the following steps to load the kernel:
 * 1. Obtains the Loaded Image Protocol to identify the boot device
 * 2. Opens the Simple File System Protocol on the boot device
 * 3. Opens the root directory of the file system
 * 4. Reads the kernel file "metalos.bin" from the root directory
 * 5. Allocates a temporary buffer and reads the kernel into it
 * 6. Copies the kernel to its final load address (KERNEL_LOAD_ADDRESS = 0x100000 = 1MB)
 * 7. Stores kernel location and size in boot_info for the kernel's use
 * 
 * @param ImageHandle Handle to the bootloader image, used to find the boot device
 * @param boot_info Pointer to BootInfo structure to receive kernel location and size
 * @return EFI_SUCCESS if kernel was loaded successfully, or an error code on failure
 * 
 * @note The kernel file must be named "metalos.bin" and located in the root directory
 *       of the boot device (typically the EFI System Partition).
 * @note The kernel is copied to physical address 0x100000 (1MB), which is a standard
 *       location above the legacy BIOS area and below the 16MB mark.
 * @note This function allocates memory using UEFI's AllocatePool, which is only valid
 *       until ExitBootServices is called. The kernel is copied to a permanent location.
 */
EFI_STATUS load_kernel(EFI_HANDLE ImageHandle, BootInfo* boot_info) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL* loaded_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;
    EFI_FILE_PROTOCOL* root = NULL;
    EFI_FILE_PROTOCOL* kernel_file = NULL;
    
    // Get loaded image protocol to find our boot device
    EFI_GUID loaded_image_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    status = gBS->HandleProtocol(ImageHandle, &loaded_image_guid, (void**)&loaded_image);
    if (status != EFI_SUCCESS) {
        return status;
    }
    
    // Open file system protocol on boot device
    EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    status = gBS->HandleProtocol(loaded_image->DeviceHandle, &fs_guid, (void**)&fs);
    if (status != EFI_SUCCESS) {
        return status;
    }
    
    // Open root directory
    status = fs->OpenVolume(fs, &root);
    if (status != EFI_SUCCESS) {
        return status;
    }
    
    // Open kernel file
    status = root->Open(root, &kernel_file, u"metalos.bin", EFI_FILE_MODE_READ, 0);
    if (status != EFI_SUCCESS) {
        root->Close(root);
        return status;
    }
    
    // Get file size
    EFI_GUID file_info_guid = EFI_FILE_INFO_GUID;
    EFI_FILE_INFO file_info;
    UINTN info_size = sizeof(EFI_FILE_INFO);
    status = kernel_file->GetInfo(kernel_file, &file_info_guid, &info_size, &file_info);
    if (status != EFI_SUCCESS) {
        kernel_file->Close(kernel_file);
        root->Close(root);
        return status;
    }
    
    UINT64 kernel_size = file_info.FileSize;
    
    // Allocate memory for kernel - use temporary buffer since UEFI may not
    // allow us to allocate at specific physical address before ExitBootServices
    VOID* kernel_buffer = NULL;
    status = gBS->AllocatePool(EfiLoaderData, kernel_size, &kernel_buffer);
    if (status != EFI_SUCCESS) {
        kernel_file->Close(kernel_file);
        root->Close(root);
        return status;
    }
    
    // Read kernel into memory
    UINTN read_size = kernel_size;
    status = kernel_file->Read(kernel_file, &read_size, kernel_buffer);
    if (status != EFI_SUCCESS || read_size != kernel_size) {
        gBS->FreePool(kernel_buffer);
        kernel_file->Close(kernel_file);
        root->Close(root);
        return EFI_LOAD_ERROR;
    }
    
    // Copy kernel to final location (KERNEL_LOAD_ADDRESS)
    // Note: This is a simplified approach. In a production bootloader,
    // we would validate that this memory region is available by checking
    // the memory map. For now, we rely on UEFI having mapped low memory.
    efi_memcpy((VOID*)KERNEL_LOAD_ADDRESS, kernel_buffer, kernel_size);
    
    // Store kernel info
    boot_info->kernel_base = KERNEL_LOAD_ADDRESS;
    boot_info->kernel_size = kernel_size;
    
    // Cleanup
    gBS->FreePool(kernel_buffer);
    kernel_file->Close(kernel_file);
    root->Close(root);
    
    return EFI_SUCCESS;
}

/**
 * @brief Retrieves the ACPI RSDP (Root System Description Pointer) from UEFI
 * 
 * The RSDP is the entry point to ACPI (Advanced Configuration and Power Interface)
 * tables, which provide information about the system hardware, including:
 * - Multiple APIC Description Table (MADT) for SMP initialization
 * - PCI routing tables
 * - Power management configuration
 * - Hardware description
 * 
 * This function searches the UEFI Configuration Table for the ACPI 2.0+ table GUID
 * and returns a pointer to the RSDP structure if found.
 * 
 * @return Pointer to the RSDP structure if found, NULL if not available
 * 
 * @note ACPI 2.0+ is preferred over ACPI 1.0 because it uses 64-bit addresses.
 * @note The RSDP pointer remains valid after ExitBootServices is called since it
 *       points to firmware-provided tables in reserved memory.
 * @note The kernel can use this to locate ACPI tables for multicore initialization
 *       and hardware discovery.
 */
void* get_rsdp(void) {
    EFI_GUID acpi_20_guid = EFI_ACPI_20_TABLE_GUID;
    
    // Search configuration tables for ACPI 2.0 table
    for (UINTN i = 0; i < gST->NumberOfTableEntries; i++) {
        if (guid_compare(&gST->ConfigurationTable[i].VendorGuid, &acpi_20_guid)) {
            return gST->ConfigurationTable[i].VendorTable;
        }
    }
    
    return NULL;
}

/**
 * @brief Main entry point for the UEFI bootloader
 * 
 * This is the entry point called by UEFI firmware when the bootloader is loaded.
 * It performs the following steps in order:
 * 
 * 1. Initialize UEFI services and boot_info structure
 * 2. Display boot banner
 * 3. Initialize graphics and get framebuffer information
 * 4. Load the kernel binary from disk
 * 5. Get ACPI RSDP for hardware information
 * 6. Get UEFI memory map
 * 7. Exit UEFI boot services (point of no return - transfers control from firmware)
 * 8. Jump to the kernel entry point
 * 
 * After ExitBootServices is called:
 * - UEFI Boot Services are no longer available
 * - UEFI Runtime Services remain available
 * - The kernel takes full control of the system
 * - No more firmware calls can be made except runtime services
 * 
 * @param ImageHandle Handle to this bootloader image, used for protocol access
 * @param SystemTable Pointer to UEFI System Table containing all UEFI services
 * @return EFI_SUCCESS on successful boot (should never return),
 *         or an error code if boot fails
 * 
 * @note If ExitBootServices fails on the first attempt, the memory map may have
 *       changed. This function automatically retries once with an updated memory map.
 * @note The kernel entry point is assumed to be at KERNEL_LOAD_ADDRESS (0x100000).
 * @note The kernel receives a pointer to the BootInfo structure containing all
 *       information needed to initialize the system.
 */
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS status;
    BootInfo boot_info;
    UINTN map_key;
    UINT32 descriptor_version;
    
    // Initialize
    gST = SystemTable;
    gBS = SystemTable->BootServices;
    efi_memset(&boot_info, 0, sizeof(BootInfo));
    
    // Print banner
    print_string(u"MetalOS v0.1 - MINIMAL BOOTLOADER\r\n");
    print_string(u"==================================\r\n\r\n");
    
    // Get framebuffer (don't care about resolution, take what UEFI gives us)
    print_string(u"Getting framebuffer...");
    status = initialize_graphics(ImageHandle, &boot_info);
    print_status(u"", status);
    if (status != EFI_SUCCESS) {
        print_string(u"WARNING: No graphics, continuing anyway...\r\n");
    }
    
    // Load kernel (just read metalos.bin, don't overthink it)
    print_string(u"Loading kernel...");
    status = load_kernel(ImageHandle, &boot_info);
    print_status(u"", status);
    if (status != EFI_SUCCESS) {
        print_string(u"ERROR: Can't load kernel\r\n");
        return status;
    }
    
    // Get RSDP for ACPI
    print_string(u"Getting ACPI RSDP...");
    boot_info.rsdp = get_rsdp();
    if (boot_info.rsdp) {
        print_string(u" [OK]\r\n");
    } else {
        print_string(u" [NOT FOUND]\r\n");
    }
    
    // Get memory map (minimal info)
    print_string(u"Getting memory map...");
    
    // First call to get size
    UINTN memory_map_size = 0;
    UINTN descriptor_size = 0;
    status = gBS->GetMemoryMap(&memory_map_size, NULL, &map_key, &descriptor_size, &descriptor_version);
    
    // Allocate buffer (add extra space for potential allocations)
    memory_map_size += 2 * descriptor_size;
    EFI_MEMORY_DESCRIPTOR* memory_map = NULL;
    status = gBS->AllocatePool(EfiLoaderData, memory_map_size, (void**)&memory_map);
    if (status != EFI_SUCCESS) {
        print_string(u" [FAILED]\r\n");
        return status;
    }
    
    // Second call to get actual memory map
    status = gBS->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
    if (status != EFI_SUCCESS) {
        print_string(u" [FAILED]\r\n");
        return status;
    }
    print_string(u" [OK]\r\n");
    
    // Store memory map info
    boot_info.memory_map = memory_map;
    boot_info.memory_map_size = memory_map_size;
    boot_info.memory_map_descriptor_size = descriptor_size;
    
    // Exit boot services (point of no return)
    print_string(u"Exiting UEFI boot services...");
    status = gBS->ExitBootServices(ImageHandle, map_key);
    if (status != EFI_SUCCESS) {
        // If this fails, memory map changed - try one more time
        gBS->FreePool(memory_map);
        
        memory_map_size = 0;
        status = gBS->GetMemoryMap(&memory_map_size, NULL, &map_key, &descriptor_size, &descriptor_version);
        memory_map_size += 2 * descriptor_size;
        
        status = gBS->AllocatePool(EfiLoaderData, memory_map_size, (VOID**)&memory_map);
        if (status != EFI_SUCCESS) {
            // Can't print after ExitBootServices fails
            return status;
        }
        
        status = gBS->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
        if (status != EFI_SUCCESS) {
            gBS->FreePool(memory_map);
            return status;
        }
        
        boot_info.memory_map = memory_map;
        boot_info.memory_map_size = memory_map_size;
        
        status = gBS->ExitBootServices(ImageHandle, map_key);
        if (status != EFI_SUCCESS) {
            // Still failed - can't continue
            return status;
        }
    }
    
    // Jump to kernel
    // Cast KERNEL_LOAD_ADDRESS to function pointer and call with boot_info
    typedef void (*kernel_entry_t)(BootInfo*);
    kernel_entry_t kernel_entry = (kernel_entry_t)KERNEL_LOAD_ADDRESS;
    kernel_entry(&boot_info);
    
    // Should never get here
    return EFI_SUCCESS;
}
