#ifndef METALOS_KERNEL_MEMORY_H
#define METALOS_KERNEL_MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include "kernel/kernel.h"

// Memory constants
#define PAGE_SIZE 4096

#ifdef __cplusplus
// C++ PhysicalMemoryManager class
class PhysicalMemoryManager {
private:
    uint8_t pageBitmap[32768];  // Supports up to 128MB with 4KB pages
    uint64_t totalPages;
    uint64_t usedPages;

public:
    PhysicalMemoryManager();
    
    void init(BootInfo* bootInfo);
    void* allocPage();
    void freePage(void* page);
    uint64_t getTotalMemory() const;
    uint64_t getFreeMemory() const;
};

// C++ HeapAllocator class
class HeapAllocator {
private:
    uint8_t* heapStart;
    uint8_t* heapCurrent;
    uint8_t* heapEnd;

public:
    HeapAllocator();
    
    void init(void* start, size_t size);
    void* alloc(size_t size);
    void* calloc(size_t num, size_t size);
    void free(void* ptr);
};

extern "C" {
#endif

// C-compatible physical memory manager
void pmm_init(BootInfo* boot_info);
void* pmm_alloc_page(void);
void pmm_free_page(void* page);
uint64_t pmm_get_total_memory(void);
uint64_t pmm_get_free_memory(void);

// C-compatible kernel heap allocator
void heap_init(void* start, size_t size);
void* kmalloc(size_t size);
void* kcalloc(size_t num, size_t size);
void kfree(void* ptr);

// Memory utility functions
void* memset(void* dest, int val, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int memcmp(const void* s1, const void* s2, size_t count);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_MEMORY_H
