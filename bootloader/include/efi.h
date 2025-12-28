#ifndef METALOS_BOOTLOADER_EFI_H
#define METALOS_BOOTLOADER_EFI_H

#include <stdint.h>

// Basic UEFI types
typedef uint64_t EFI_STATUS;
typedef void*    EFI_HANDLE;
typedef uint64_t UINTN;
typedef uint16_t CHAR16;

// EFI Status codes
#define EFI_SUCCESS                 0
#define EFI_LOAD_ERROR              1
#define EFI_INVALID_PARAMETER       2
#define EFI_UNSUPPORTED             3
#define EFI_BUFFER_TOO_SMALL        5
#define EFI_NOT_READY               6
#define EFI_NOT_FOUND               14

// EFI Memory types
#define EfiReservedMemoryType       0
#define EfiLoaderCode               1
#define EfiLoaderData               2
#define EfiBootServicesCode         3
#define EfiBootServicesData         4
#define EfiRuntimeServicesCode      5
#define EfiRuntimeServicesData      6
#define EfiConventionalMemory       7
#define EfiUnusableMemory           8
#define EfiACPIReclaimMemory        9
#define EfiACPIMemoryNVS            10
#define EfiMemoryMappedIO           11
#define EfiMemoryMappedIOPortSpace  12
#define EfiPalCode                  13
#define EfiPersistentMemory         14

typedef struct {
    uint32_t Type;
    uint64_t PhysicalStart;
    uint64_t VirtualStart;
    uint64_t NumberOfPages;
    uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

// Graphics Output Protocol structures
typedef struct {
    uint32_t RedMask;
    uint32_t GreenMask;
    uint32_t BlueMask;
    uint32_t ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    uint32_t                    Version;
    uint32_t                    HorizontalResolution;
    uint32_t                    VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT   PixelFormat;
    EFI_PIXEL_BITMASK           PixelInformation;
    uint32_t                    PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    uint32_t                                MaxMode;
    uint32_t                                Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION*   Info;
    UINTN                                   SizeOfInfo;
    uint64_t                                FrameBufferBase;
    UINTN                                   FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// Boot information passed to kernel
typedef struct {
    uint64_t memory_map_size;
    uint64_t memory_map_descriptor_size;
    EFI_MEMORY_DESCRIPTOR* memory_map;
    
    uint64_t framebuffer_base;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_bpp;
    
    uint64_t kernel_base;
    uint64_t kernel_size;
    
    void* rsdp; // ACPI RSDP pointer
} BootInfo;

#endif // METALOS_BOOTLOADER_EFI_H
