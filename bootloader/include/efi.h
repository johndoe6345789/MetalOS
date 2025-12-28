#ifndef METALOS_BOOTLOADER_EFI_H
#define METALOS_BOOTLOADER_EFI_H

#include <stdint.h>
#include <stddef.h>

// Basic UEFI types
typedef uint64_t EFI_STATUS;
typedef void*    EFI_HANDLE;
typedef uint64_t UINTN;
typedef uint64_t UINT64;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t  UINT8;
typedef int64_t  INTN;
typedef uint16_t CHAR16;
typedef uint8_t  BOOLEAN;
typedef void     VOID;

#define TRUE  1
#define FALSE 0

// EFI GUID structure
typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8  Data4[8];
} EFI_GUID;

// EFI Status codes
#define EFI_SUCCESS                 0
#define EFI_LOAD_ERROR              1
#define EFI_INVALID_PARAMETER       2
#define EFI_UNSUPPORTED             3
#define EFI_BUFFER_TOO_SMALL        5
#define EFI_NOT_READY               6
#define EFI_NOT_FOUND               14

// EFI Memory types
typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef struct {
    UINT32 Type;
    UINT64 PhysicalStart;
    UINT64 VirtualStart;
    UINT64 NumberOfPages;
    UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR;

// EFI Table Header
typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

// Graphics Output Protocol structures
typedef struct {
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    UINT32                    Version;
    UINT32                    HorizontalResolution;
    UINT32                    VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK         PixelInformation;
    UINT32                    PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32                                MaxMode;
    UINT32                                Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info;
    UINTN                                 SizeOfInfo;
    UINT64                                FrameBufferBase;
    UINTN                                 FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// Forward declarations
struct _EFI_GRAPHICS_OUTPUT_PROTOCOL;
struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
struct _EFI_FILE_PROTOCOL;

// Graphics Output Protocol
typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE)(
    struct _EFI_GRAPHICS_OUTPUT_PROTOCOL*     This,
    UINT32                                    ModeNumber,
    UINTN*                                    SizeOfInfo,
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION**    Info
);

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE)(
    struct _EFI_GRAPHICS_OUTPUT_PROTOCOL*     This,
    UINT32                                    ModeNumber
);

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE   QueryMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE     SetMode;
    VOID*                                     Blt;  // We don't need this
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE*        Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

// Simple Text Output Protocol
typedef EFI_STATUS (*EFI_TEXT_STRING)(
    struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*  This,
    CHAR16*                                   String
);

typedef EFI_STATUS (*EFI_TEXT_RESET)(
    struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*  This,
    BOOLEAN                                   ExtendedVerification
);

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET                            Reset;
    EFI_TEXT_STRING                           OutputString;
    VOID*                                     TestString;
    VOID*                                     QueryMode;
    VOID*                                     SetMode;
    VOID*                                     SetAttribute;
    VOID*                                     ClearScreen;
    VOID*                                     SetCursorPosition;
    VOID*                                     EnableCursor;
    VOID*                                     Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// File Protocol
#define EFI_FILE_MODE_READ   0x0000000000000001
#define EFI_FILE_MODE_WRITE  0x0000000000000002
#define EFI_FILE_MODE_CREATE 0x8000000000000000

typedef EFI_STATUS (*EFI_FILE_OPEN)(
    struct _EFI_FILE_PROTOCOL*                This,
    struct _EFI_FILE_PROTOCOL**               NewHandle,
    CHAR16*                                   FileName,
    UINT64                                    OpenMode,
    UINT64                                    Attributes
);

typedef EFI_STATUS (*EFI_FILE_CLOSE)(
    struct _EFI_FILE_PROTOCOL*                This
);

typedef EFI_STATUS (*EFI_FILE_READ)(
    struct _EFI_FILE_PROTOCOL*                This,
    UINTN*                                    BufferSize,
    VOID*                                     Buffer
);

typedef EFI_STATUS (*EFI_FILE_GET_INFO)(
    struct _EFI_FILE_PROTOCOL*                This,
    EFI_GUID*                                 InformationType,
    UINTN*                                    BufferSize,
    VOID*                                     Buffer
);

typedef struct _EFI_FILE_PROTOCOL {
    UINT64                                    Revision;
    EFI_FILE_OPEN                             Open;
    EFI_FILE_CLOSE                            Close;
    VOID*                                     Delete;
    EFI_FILE_READ                             Read;
    VOID*                                     Write;
    VOID*                                     GetPosition;
    VOID*                                     SetPosition;
    EFI_FILE_GET_INFO                         GetInfo;
    VOID*                                     SetInfo;
    VOID*                                     Flush;
} EFI_FILE_PROTOCOL;

// Simple File System Protocol
typedef EFI_STATUS (*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(
    struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*  This,
    EFI_FILE_PROTOCOL**                       Root
);

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64                                            Revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME       OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// Loaded Image Protocol
typedef struct {
    UINT32              Revision;
    EFI_HANDLE          ParentHandle;
    VOID*               SystemTable;
    EFI_HANDLE          DeviceHandle;
    VOID*               FilePath;
    VOID*               Reserved;
    UINT32              LoadOptionsSize;
    VOID*               LoadOptions;
    VOID*               ImageBase;
    UINT64              ImageSize;
    EFI_MEMORY_TYPE     ImageCodeType;
    EFI_MEMORY_TYPE     ImageDataType;
    VOID*               Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

// Protocol GUIDs
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    { 0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a} }

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
    { 0x964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b} }

#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
    { 0x5b1b31a1, 0x9562, 0x11d2, {0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b} }

#define EFI_FILE_INFO_GUID \
    { 0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b} }

#define EFI_ACPI_20_TABLE_GUID \
    { 0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81} }

// Forward declarations
struct _EFI_BOOT_SERVICES;
struct _EFI_SYSTEM_TABLE;

// Boot Services functions
typedef EFI_STATUS (*EFI_LOCATE_PROTOCOL)(
    EFI_GUID*                                 Protocol,
    VOID*                                     Registration,
    VOID**                                    Interface
);

typedef EFI_STATUS (*EFI_HANDLE_PROTOCOL)(
    EFI_HANDLE                                Handle,
    EFI_GUID*                                 Protocol,
    VOID**                                    Interface
);

typedef EFI_STATUS (*EFI_GET_MEMORY_MAP)(
    UINTN*                                    MemoryMapSize,
    EFI_MEMORY_DESCRIPTOR*                    MemoryMap,
    UINTN*                                    MapKey,
    UINTN*                                    DescriptorSize,
    UINT32*                                   DescriptorVersion
);

typedef EFI_STATUS (*EFI_ALLOCATE_POOL)(
    EFI_MEMORY_TYPE                           PoolType,
    UINTN                                     Size,
    VOID**                                    Buffer
);

typedef EFI_STATUS (*EFI_FREE_POOL)(
    VOID*                                     Buffer
);

typedef EFI_STATUS (*EFI_EXIT_BOOT_SERVICES)(
    EFI_HANDLE                                ImageHandle,
    UINTN                                     MapKey
);

// Boot Services Table
typedef struct _EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER                          Hdr;
    
    // Task Priority Services (stub pointers)
    VOID*                                     RaiseTPL;
    VOID*                                     RestoreTPL;
    
    // Memory Services (stub pointers for unused)
    VOID*                                     AllocatePages;
    VOID*                                     FreePages;
    EFI_GET_MEMORY_MAP                        GetMemoryMap;
    EFI_ALLOCATE_POOL                         AllocatePool;
    EFI_FREE_POOL                             FreePool;
    
    // Event & Timer Services (stub pointers)
    VOID*                                     CreateEvent;
    VOID*                                     SetTimer;
    VOID*                                     WaitForEvent;
    VOID*                                     SignalEvent;
    VOID*                                     CloseEvent;
    VOID*                                     CheckEvent;
    
    // Protocol Handler Services (stub pointers for unused)
    VOID*                                     InstallProtocolInterface;
    VOID*                                     ReinstallProtocolInterface;
    VOID*                                     UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL                       HandleProtocol;
    VOID*                                     Reserved;
    VOID*                                     RegisterProtocolNotify;
    VOID*                                     LocateHandle;
    VOID*                                     LocateDevicePath;
    VOID*                                     InstallConfigurationTable;
    
    // Image Services (stub pointers)
    VOID*                                     LoadImage;
    VOID*                                     StartImage;
    VOID*                                     Exit;
    VOID*                                     UnloadImage;
    EFI_EXIT_BOOT_SERVICES                    ExitBootServices;
    
    // Miscellaneous Services (stub pointers)
    VOID*                                     GetNextMonotonicCount;
    VOID*                                     Stall;
    VOID*                                     SetWatchdogTimer;
    
    // Driver Support Services (stub pointers)
    VOID*                                     ConnectController;
    VOID*                                     DisconnectController;
    
    // Open and Close Protocol Services (stub pointers)
    VOID*                                     OpenProtocol;
    VOID*                                     CloseProtocol;
    VOID*                                     OpenProtocolInformation;
    
    // Library Services (stub pointers)
    VOID*                                     ProtocolsPerHandle;
    VOID*                                     LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL                       LocateProtocol;
    VOID*                                     InstallMultipleProtocolInterfaces;
    VOID*                                     UninstallMultipleProtocolInterfaces;
    
    // CRC Services (stub pointers)
    VOID*                                     CalculateCrc32;
    
    // Miscellaneous Services (stub pointers)
    VOID*                                     CopyMem;
    VOID*                                     SetMem;
    VOID*                                     CreateEventEx;
} EFI_BOOT_SERVICES;

// Configuration Table
typedef struct {
    EFI_GUID                                  VendorGuid;
    VOID*                                     VendorTable;
} EFI_CONFIGURATION_TABLE;

// System Table
typedef struct _EFI_SYSTEM_TABLE {
    EFI_TABLE_HEADER                          Hdr;
    CHAR16*                                   FirmwareVendor;
    UINT32                                    FirmwareRevision;
    EFI_HANDLE                                ConsoleInHandle;
    VOID*                                     ConIn;
    EFI_HANDLE                                ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*          ConOut;
    EFI_HANDLE                                StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*          StdErr;
    VOID*                                     RuntimeServices;
    EFI_BOOT_SERVICES*                        BootServices;
    UINTN                                     NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE*                  ConfigurationTable;
} EFI_SYSTEM_TABLE;

// File Info structure
typedef struct {
    UINT64                                    Size;
    UINT64                                    FileSize;
    UINT64                                    PhysicalSize;
    VOID*                                     CreateTime;
    VOID*                                     LastAccessTime;
    VOID*                                     ModificationTime;
    UINT64                                    Attribute;
    CHAR16                                    FileName[256];
} EFI_FILE_INFO;

// Boot information passed to kernel
typedef struct {
    UINT64                                    memory_map_size;
    UINT64                                    memory_map_descriptor_size;
    EFI_MEMORY_DESCRIPTOR*                    memory_map;
    
    UINT64                                    framebuffer_base;
    UINT32                                    framebuffer_width;
    UINT32                                    framebuffer_height;
    UINT32                                    framebuffer_pitch;
    UINT32                                    framebuffer_bpp;
    
    UINT64                                    kernel_base;
    UINT64                                    kernel_size;
    
    VOID*                                     rsdp; // ACPI RSDP pointer
} BootInfo;

#endif // METALOS_BOOTLOADER_EFI_H
