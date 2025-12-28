#ifndef METALOS_KERNEL_KERNEL_H
#define METALOS_KERNEL_KERNEL_H

#include <stdint.h>

// Kernel version
#define KERNEL_VERSION_MAJOR 0
#define KERNEL_VERSION_MINOR 1
#define KERNEL_VERSION_PATCH 0
#define KERNEL_NAME "MetalOS"

// Boot information structure (matches bootloader)
typedef struct {
    uint64_t memory_map_size;
    uint64_t memory_map_descriptor_size;
    void* memory_map;
    
    uint64_t framebuffer_base;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_bpp;
    
    uint64_t kernel_base;
    uint64_t kernel_size;
    
    void* rsdp;
} BootInfo;

#ifdef __cplusplus
extern "C" {
#endif

// Kernel entry point
void kernel_main(BootInfo* boot_info);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_KERNEL_H
