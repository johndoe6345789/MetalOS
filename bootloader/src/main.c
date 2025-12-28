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

// Helper: Compare GUIDs
static int guid_compare(const EFI_GUID* a, const EFI_GUID* b) {
    if (a->Data1 != b->Data1) return 0;
    if (a->Data2 != b->Data2) return 0;
    if (a->Data3 != b->Data3) return 0;
    for (int i = 0; i < 8; i++) {
        if (a->Data4[i] != b->Data4[i]) return 0;
    }
    return 1;
}

// Helper: Memory set
static void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

// Helper: Memory copy
static void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) *d++ = *s++;
    return dest;
}

/*
 * Print a string to the UEFI console
 */
void print_string(const CHAR16* str) {
    if (gST && gST->ConOut) {
        gST->ConOut->OutputString(gST->ConOut, (CHAR16*)str);
    }
}

/*
 * Print operation status
 */
void print_status(const CHAR16* operation, EFI_STATUS status) {
    print_string(operation);
    if (status == EFI_SUCCESS) {
        print_string(u" [OK]\r\n");
    } else {
        print_string(u" [FAILED]\r\n");
    }
}

/*
 * Initialize graphics output protocol
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

/*
 * Load kernel from disk
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
    
    // Allocate memory for kernel at KERNEL_LOAD_ADDRESS
    // We'll just use AllocatePool for simplicity
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
    
    // Copy kernel to final location
    memcpy((void*)KERNEL_LOAD_ADDRESS, kernel_buffer, kernel_size);
    
    // Store kernel info
    boot_info->kernel_base = KERNEL_LOAD_ADDRESS;
    boot_info->kernel_size = kernel_size;
    
    // Cleanup
    gBS->FreePool(kernel_buffer);
    kernel_file->Close(kernel_file);
    root->Close(root);
    
    return EFI_SUCCESS;
}

/*
 * Get ACPI RSDP (Root System Description Pointer)
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

/*
 * Main entry point for bootloader
 */
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS status;
    BootInfo boot_info;
    UINTN map_key;
    UINT32 descriptor_version;
    
    // Initialize
    gST = SystemTable;
    gBS = SystemTable->BootServices;
    memset(&boot_info, 0, sizeof(BootInfo));
    
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
        gBS->GetMemoryMap(&memory_map_size, NULL, &map_key, &descriptor_size, &descriptor_version);
        memory_map_size += 2 * descriptor_size;
        gBS->AllocatePool(EfiLoaderData, memory_map_size, (void**)&memory_map);
        gBS->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
        
        boot_info.memory_map = memory_map;
        boot_info.memory_map_size = memory_map_size;
        
        status = gBS->ExitBootServices(ImageHandle, map_key);
        if (status != EFI_SUCCESS) {
            // Can't print after this point if it fails
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
