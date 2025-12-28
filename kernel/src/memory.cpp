/*
 * MetalOS Kernel - Memory Management
 * 
 * Simple physical memory manager and heap allocator
 * Minimal implementation for single-app OS
 */

#include "kernel/memory.h"

// Physical memory bitmap constants
#define BITMAP_SIZE 32768  // Supports up to 128MB with 4KB pages

// PhysicalMemoryManager class implementation
PhysicalMemoryManager::PhysicalMemoryManager() 
    : totalPages(0), usedPages(0) {
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        pageBitmap[i] = 0;
    }
}

void PhysicalMemoryManager::init(BootInfo* bootInfo) {
    (void)bootInfo;  // TODO: Parse UEFI memory map
    
    // For now, assume 128MB of usable memory starting at 16MB
    totalPages = (128 * 1024 * 1024) / PAGE_SIZE;
    
    // Clear bitmap
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        pageBitmap[i] = 0;
    }
    
    usedPages = 0;
}

void* PhysicalMemoryManager::allocPage() {
    // Find first free page in bitmap
    for (uint64_t i = 0; i < totalPages; i++) {
        uint64_t byte = i / 8;
        uint64_t bit = i % 8;
        
        if (!(pageBitmap[byte] & (1 << bit))) {
            // Mark as used
            pageBitmap[byte] |= (1 << bit);
            usedPages++;
            
            // Return physical address
            // Assuming memory starts at 16MB
            return (void*)((0x01000000UL) + (i * PAGE_SIZE));
        }
    }
    
    // Out of memory
    return nullptr;
}

void PhysicalMemoryManager::freePage(void* page) {
    uint64_t addr = (uint64_t)page;
    
    // Calculate page index
    uint64_t pageIdx = (addr - 0x01000000UL) / PAGE_SIZE;
    
    if (pageIdx >= totalPages) {
        return;  // Invalid address
    }
    
    uint64_t byte = pageIdx / 8;
    uint64_t bit = pageIdx % 8;
    
    // Mark as free
    pageBitmap[byte] &= ~(1 << bit);
    usedPages--;
}

uint64_t PhysicalMemoryManager::getTotalMemory() const {
    return totalPages * PAGE_SIZE;
}

uint64_t PhysicalMemoryManager::getFreeMemory() const {
    return (totalPages - usedPages) * PAGE_SIZE;
}

// HeapAllocator class implementation
HeapAllocator::HeapAllocator() 
    : heapStart(nullptr), heapCurrent(nullptr), heapEnd(nullptr) {}

void HeapAllocator::init(void* start, size_t size) {
    heapStart = (uint8_t*)start;
    heapCurrent = heapStart;
    heapEnd = heapStart + size;
}

void* HeapAllocator::alloc(size_t size) {
    if (!heapStart) {
        return nullptr;
    }
    
    // Align to 16 bytes
    size = (size + 15) & ~15;
    
    if (heapCurrent + size > heapEnd) {
        return nullptr;  // Out of heap memory
    }
    
    void* ptr = heapCurrent;
    heapCurrent += size;
    
    return ptr;
}

void* HeapAllocator::calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = alloc(total);
    
    if (ptr) {
        memset(ptr, 0, total);
    }
    
    return ptr;
}

void HeapAllocator::free(void* ptr) {
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

// Global instances
static PhysicalMemoryManager globalPMM;
static HeapAllocator globalHeap;

// C-compatible wrapper functions
extern "C" {

void pmm_init(BootInfo* boot_info) {
    globalPMM.init(boot_info);
}

void* pmm_alloc_page(void) {
    return globalPMM.allocPage();
}

void pmm_free_page(void* page) {
    globalPMM.freePage(page);
}

uint64_t pmm_get_total_memory(void) {
    return globalPMM.getTotalMemory();
}

uint64_t pmm_get_free_memory(void) {
    return globalPMM.getFreeMemory();
}

void heap_init(void* start, size_t size) {
    globalHeap.init(start, size);
}

void* kmalloc(size_t size) {
    return globalHeap.alloc(size);
}

void* kcalloc(size_t num, size_t size) {
    return globalHeap.calloc(num, size);
}

void kfree(void* ptr) {
    globalHeap.free(ptr);
}

} // extern "C"
