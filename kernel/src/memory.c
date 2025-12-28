/*
 * MetalOS Kernel - Memory Management
 * 
 * Simple physical memory manager and heap allocator
 * Minimal implementation for single-app OS
 */

#include "kernel/memory.h"

// Physical memory bitmap
#define PAGE_SIZE 4096
#define BITMAP_SIZE 32768  // Supports up to 128MB with 4KB pages

static uint8_t page_bitmap[BITMAP_SIZE];
static uint64_t total_pages = 0;
static uint64_t used_pages = 0;

// Heap for kernel allocations
static uint8_t* heap_start = NULL;
static uint8_t* heap_current = NULL;
static uint8_t* heap_end = NULL;

// Initialize physical memory manager
void pmm_init(BootInfo* boot_info) {
    (void)boot_info;  // TODO: Parse UEFI memory map
    
    // For now, assume 128MB of usable memory starting at 16MB
    total_pages = (128 * 1024 * 1024) / PAGE_SIZE;
    
    // Clear bitmap
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        page_bitmap[i] = 0;
    }
    
    used_pages = 0;
}

// Allocate a physical page
void* pmm_alloc_page(void) {
    // Find first free page in bitmap
    for (uint64_t i = 0; i < total_pages; i++) {
        uint64_t byte = i / 8;
        uint64_t bit = i % 8;
        
        if (!(page_bitmap[byte] & (1 << bit))) {
            // Mark as used
            page_bitmap[byte] |= (1 << bit);
            used_pages++;
            
            // Return physical address
            // Assuming memory starts at 16MB
            return (void*)((0x01000000UL) + (i * PAGE_SIZE));
        }
    }
    
    // Out of memory
    return NULL;
}

// Free a physical page
void pmm_free_page(void* page) {
    uint64_t addr = (uint64_t)page;
    
    // Calculate page index
    uint64_t page_idx = (addr - 0x01000000UL) / PAGE_SIZE;
    
    if (page_idx >= total_pages) {
        return;  // Invalid address
    }
    
    uint64_t byte = page_idx / 8;
    uint64_t bit = page_idx % 8;
    
    // Mark as free
    page_bitmap[byte] &= ~(1 << bit);
    used_pages--;
}

// Get total memory
uint64_t pmm_get_total_memory(void) {
    return total_pages * PAGE_SIZE;
}

// Get free memory
uint64_t pmm_get_free_memory(void) {
    return (total_pages - used_pages) * PAGE_SIZE;
}

// Initialize heap allocator
void heap_init(void* start, size_t size) {
    heap_start = (uint8_t*)start;
    heap_current = heap_start;
    heap_end = heap_start + size;
}

// Simple bump allocator (no free support in this version)
void* kmalloc(size_t size) {
    if (!heap_start) {
        return NULL;
    }
    
    // Align to 16 bytes
    size = (size + 15) & ~15;
    
    if (heap_current + size > heap_end) {
        return NULL;  // Out of heap memory
    }
    
    void* ptr = heap_current;
    heap_current += size;
    
    return ptr;
}

// Allocate and zero memory
void* kcalloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = kmalloc(total);
    
    if (ptr) {
        memset(ptr, 0, total);
    }
    
    return ptr;
}

// Free memory (not implemented in bump allocator)
void kfree(void* ptr) {
    (void)ptr;
    // TODO: Implement proper free with a real allocator
    // For now, bump allocator doesn't support freeing
}

// Memory utility functions

void* memset(void* dest, int val, size_t count) {
    uint8_t* d = (uint8_t*)dest;
    uint8_t v = (uint8_t)val;
    
    for (size_t i = 0; i < count; i++) {
        d[i] = v;
    }
    
    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t count) {
    const uint8_t* a = (const uint8_t*)s1;
    const uint8_t* b = (const uint8_t*)s2;
    
    for (size_t i = 0; i < count; i++) {
        if (a[i] != b[i]) {
            return a[i] - b[i];
        }
    }
    
    return 0;
}
