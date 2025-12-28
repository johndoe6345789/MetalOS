/**
 * @file memory.cpp
 * @brief Implementation of physical memory manager and kernel heap allocator
 * 
 * Provides two memory management subsystems:
 * 1. Physical Memory Manager (PMM): Manages 4KB pages using a bitmap
 * 2. Heap Allocator: Simple bump allocator for kernel dynamic allocation
 */

#include "kernel/memory.h"

// Physical memory bitmap constants
#define BITMAP_SIZE 2097152  // Supports up to 64GB with 4KB pages (64GB / 4KB = 16M pages, 16M bits = 2MB bitmap)

/* PhysicalMemoryManager class implementation */

/**
 * @brief Constructor - initializes all fields and clears bitmap
 */
PhysicalMemoryManager::PhysicalMemoryManager() 
    : totalPages(0), usedPages(0) {
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        pageBitmap[i] = 0;
    }
}

/**
 * @brief Initialize the physical memory manager
 * 
 * Currently uses a simplified approach:
 * - Assumes up to 64GB of usable RAM starting at physical address 16MB (0x01000000)
 * - Clears the entire page bitmap to mark all pages as free
 * - TODO: Parse the UEFI memory map from bootInfo to properly detect available memory
 * 
 * The 16MB starting address is chosen to avoid:
 * - First 1MB: Legacy BIOS area, video memory, etc.
 * - 1MB-16MB: Kernel code, boot structures, and reserved areas
 * 
 * @param bootInfo Boot information structure (currently unused, TODO: parse memory map)
 */
void PhysicalMemoryManager::init(BootInfo* bootInfo) {
    (void)bootInfo;  // TODO: Parse UEFI memory map
    
    // For now, assume 64GB of usable memory starting at 16MB
    // This supports any amount up to 64GB
    totalPages = (64ULL * 1024 * 1024 * 1024) / PAGE_SIZE;
    
    // Clear bitmap
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        pageBitmap[i] = 0;
    }
    
    usedPages = 0;
}

/**
 * @brief Allocate a single 4KB physical memory page
 * 
 * Uses a simple first-fit algorithm:
 * 1. Scan the bitmap from the beginning
 * 2. Find the first page where the corresponding bit is 0 (free)
 * 3. Set the bit to 1 (allocated)
 * 4. Calculate and return the physical address
 * 
 * Each bit in the bitmap represents one 4KB page:
 * - Byte N, Bit M represents page (N*8 + M)
 * - Physical address = 0x01000000 + (page_index * 4096)
 * 
 * @return Physical address of allocated page, or nullptr if out of memory
 */
void* PhysicalMemoryManager::allocPage() {
    // Find first free page in bitmap
    for (uint64_t i = 0; i < totalPages; i++) {
        uint64_t byte = i / 8;
        uint64_t bit = i % 8;
        
        // Bounds check to prevent buffer overflow
        if (byte >= BITMAP_SIZE) {
            break;
        }
        
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

/**
 * @brief Free a previously allocated physical memory page
 * 
 * Calculates the page index from the physical address and clears the
 * corresponding bit in the bitmap to mark the page as free.
 * 
 * @param page Physical address of page to free
 * @note Does nothing if address is invalid (< base or >= limit)
 */
void PhysicalMemoryManager::freePage(void* page) {
    uint64_t addr = (uint64_t)page;
    
    // Calculate page index
    uint64_t pageIdx = (addr - 0x01000000UL) / PAGE_SIZE;
    
    if (pageIdx >= totalPages) {
        return;  // Invalid address
    }
    
    uint64_t byte = pageIdx / 8;
    uint64_t bit = pageIdx % 8;
    
    // Bounds check to prevent buffer overflow
    if (byte >= BITMAP_SIZE) {
        return;
    }
    
    // Mark as free
    pageBitmap[byte] &= ~(1 << bit);
    usedPages--;
}

/**
 * @brief Get total memory managed by PMM in bytes
 * @return Total memory size (totalPages * PAGE_SIZE)
 */
uint64_t PhysicalMemoryManager::getTotalMemory() const {
    return totalPages * PAGE_SIZE;
}

/**
 * @brief Get free memory available in bytes
 * @return Free memory size ((totalPages - usedPages) * PAGE_SIZE)
 */
uint64_t PhysicalMemoryManager::getFreeMemory() const {
    return (totalPages - usedPages) * PAGE_SIZE;
}

/* HeapAllocator class implementation */

/**
 * @brief Constructor - initializes all pointers to null
 */
HeapAllocator::HeapAllocator() 
    : heapStart(nullptr), heapCurrent(nullptr), heapEnd(nullptr) {}

/**
 * @brief Initialize heap with a pre-allocated memory region
 * 
 * The heap operates on a contiguous region of memory. The heapCurrent pointer
 * starts at the beginning and moves forward with each allocation.
 * 
 * @param start Starting address of heap region (obtained from PMM)
 * @param size Size of heap region in bytes (e.g., 1MB = 256 pages * 4KB)
 */
void HeapAllocator::init(void* start, size_t size) {
    heapStart = (uint8_t*)start;
    heapCurrent = heapStart;
    heapEnd = heapStart + size;
}

/**
 * @brief Allocate memory from the heap (bump allocator)
 * 
 * This is a "bump" or "arena" allocator - the simplest possible allocator.
 * It just moves the current pointer forward by the requested size.
 * 
 * Process:
 * 1. Round size up to 16-byte boundary for alignment
 * 2. Check if enough space remains in heap
 * 3. Save current pointer as return value
 * 4. Move current pointer forward by aligned size
 * 
 * Alignment to 16 bytes ensures:
 * - Compatibility with SSE/AVX instructions (require 16-byte alignment)
 * - Better cache line utilization
 * - Prevents unaligned access penalties
 * 
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or nullptr if out of heap space
 */
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

/**
 * @brief Allocate and zero-initialize array memory
 * 
 * Equivalent to alloc(num * size) followed by memset to zero.
 * 
 * @param num Number of elements
 * @param size Size of each element in bytes
 * @return Pointer to allocated and zeroed memory, or nullptr if out of space
 */
void* HeapAllocator::calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = alloc(total);
    
    if (ptr) {
        memset(ptr, 0, total);
    }
    
    return ptr;
}

/**
 * @brief Free memory (no-op in bump allocator)
 * 
 * Bump allocators cannot free individual allocations. The entire heap
 * can only be reset at once. For a simple single-application OS, this
 * limitation is acceptable.
 * 
 * @param ptr Pointer to memory (ignored)
 * @todo Replace with proper allocator if individual free() is needed
 */
void HeapAllocator::free(void* ptr) {
    (void)ptr;
    // Bump allocator doesn't support freeing individual allocations
}

/* Memory utility functions */

/**
 * @brief Fill memory with a constant byte value
 * 
 * Simple byte-by-byte memset implementation. Not optimized for large blocks,
 * but sufficient for kernel use with small structures and buffers.
 * 
 * @param dest Pointer to memory block to fill
 * @param val Value to set (converted to unsigned char)
 * @param count Number of bytes to set
 * @return Pointer to dest
 */
void* memset(void* dest, int val, size_t count) {
    uint8_t* d = (uint8_t*)dest;
    uint8_t v = (uint8_t)val;
    
    for (size_t i = 0; i < count; i++) {
        d[i] = v;
    }
    
    return dest;
}

/**
 * @brief Copy memory from source to destination
 * 
 * Simple byte-by-byte memcpy implementation. Memory areas must not overlap.
 * 
 * @param dest Pointer to destination buffer
 * @param src Pointer to source buffer
 * @param count Number of bytes to copy
 * @return Pointer to dest
 * @warning Memory regions must not overlap (use memmove if they might)
 */
void* memcpy(void* dest, const void* src, size_t count) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

/**
 * @brief Compare two memory blocks
 * 
 * Compares memory byte-by-byte until a difference is found or count bytes
 * have been compared.
 * 
 * @param s1 Pointer to first memory block
 * @param s2 Pointer to second memory block
 * @param count Number of bytes to compare
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
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
