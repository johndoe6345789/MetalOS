#ifndef METALOS_KERNEL_MEMORY_H
#define METALOS_KERNEL_MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include "kernel/kernel.h"

// Memory constants
#define PAGE_SIZE 4096

// Physical memory manager
void pmm_init(BootInfo* boot_info);
void* pmm_alloc_page(void);
void pmm_free_page(void* page);
uint64_t pmm_get_total_memory(void);
uint64_t pmm_get_free_memory(void);

// Simple kernel heap allocator (bump allocator for now)
void heap_init(void* start, size_t size);
void* kmalloc(size_t size);
void* kcalloc(size_t num, size_t size);
void kfree(void* ptr);

// Memory utility functions
void* memset(void* dest, int val, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int memcmp(const void* s1, const void* s2, size_t count);

#endif // METALOS_KERNEL_MEMORY_H
