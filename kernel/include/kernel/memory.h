#ifndef METALOS_KERNEL_MEMORY_H
#define METALOS_KERNEL_MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include "kernel/kernel.h"

// Memory constants
#define PAGE_SIZE 4096

#ifdef __cplusplus
/**
 * @class PhysicalMemoryManager
 * @brief Manages physical memory pages using a bitmap-based allocator
 * 
 * This class implements a simple physical memory manager that tracks available
 * 4KB pages using a bitmap. Each bit in the bitmap represents one page:
 * - 0 = page is free and available for allocation
 * - 1 = page is in use
 * 
 * The bitmap supports up to 64GB of physical memory (2097152 bytes * 8 bits/byte
 * * 4KB per page = 64GB). Memory is assumed to start at physical address 0x01000000
 * (16MB) to avoid conflicts with legacy hardware and the kernel itself.
 * 
 * This is a very simple allocator suitable for a minimal kernel. It does not:
 * - Handle memory regions with different properties
 * - Support allocation of multiple contiguous pages at once
 * - Track memory usage per process (single application OS)
 */
class PhysicalMemoryManager {
private:
    uint8_t pageBitmap[2097152];  ///< Bitmap tracking page allocation (64GB / 4KB pages = 16M pages = 2MB bitmap)
    uint64_t totalPages;         ///< Total number of pages managed
    uint64_t usedPages;          ///< Number of pages currently allocated

public:
    /** @brief Constructor - initializes bitmap to all zeros (all pages free) */
    PhysicalMemoryManager();
    
    /**
     * @brief Initialize the physical memory manager with boot information
     * @param bootInfo Boot information from bootloader (currently unused, assumes up to 64GB)
     * @note Currently hardcoded to manage up to 64GB starting at 16MB physical address
     */
    void init(BootInfo* bootInfo);
    
    /**
     * @brief Allocate a single 4KB page from physical memory
     * @return Physical address of allocated page, or nullptr if out of memory
     * @note Returns first available page found in bitmap (no specific allocation strategy)
     */
    void* allocPage();
    
    /**
     * @brief Free a previously allocated page, returning it to the available pool
     * @param page Physical address of the page to free
     * @note Does nothing if the page address is invalid or out of managed range
     */
    void freePage(void* page);
    
    /**
     * @brief Get total amount of physical memory managed (in bytes)
     * @return Total memory in bytes (totalPages * PAGE_SIZE)
     */
    uint64_t getTotalMemory() const;
    
    /**
     * @brief Get amount of free physical memory available (in bytes)
     * @return Free memory in bytes ((totalPages - usedPages) * PAGE_SIZE)
     */
    uint64_t getFreeMemory() const;
};

/**
 * @class HeapAllocator
 * @brief Simple bump allocator for kernel heap memory
 * 
 * This class implements a very simple "bump" or "arena" allocator. Memory is
 * allocated by simply incrementing a pointer (heapCurrent) forward. This is
 * extremely fast but has limitations:
 * - Cannot free individual allocations (free() is a no-op)
 * - Memory is only reclaimed when the entire heap is reset
 * - Can fragment if many small allocations are made
 * 
 * For a single-application OS that doesn't need complex memory management,
 * this simple allocator is sufficient and has minimal code size.
 * 
 * All allocations are aligned to 16-byte boundaries for performance and
 * compatibility with SSE/AVX instructions.
 */
class HeapAllocator {
private:
    uint8_t* heapStart;   ///< Start address of the heap region
    uint8_t* heapCurrent; ///< Current allocation pointer (bump pointer)
    uint8_t* heapEnd;     ///< End address of the heap region (exclusive)

public:
    /** @brief Constructor - initializes all pointers to null */
    HeapAllocator();
    
    /**
     * @brief Initialize the heap allocator with a memory region
     * @param start Starting address of heap memory region
     * @param size Size of heap region in bytes
     * @note The memory region should be obtained from the physical memory manager
     */
    void init(void* start, size_t size);
    
    /**
     * @brief Allocate memory from the heap
     * @param size Number of bytes to allocate
     * @return Pointer to allocated memory, or nullptr if out of heap space
     * @note Allocation is aligned to 16-byte boundaries
     * @note This is a bump allocator - simply moves heapCurrent forward
     */
    void* alloc(size_t size);
    
    /**
     * @brief Allocate and zero-initialize an array of objects
     * @param num Number of elements
     * @param size Size of each element in bytes
     * @return Pointer to allocated and zeroed memory, or nullptr if out of space
     * @note Equivalent to alloc(num * size) followed by memset to zero
     */
    void* calloc(size_t num, size_t size);
    
    /**
     * @brief Free allocated memory (no-op in bump allocator)
     * @param ptr Pointer to memory to free (ignored)
     * @note This function does nothing - bump allocators cannot free individual allocations
     * @todo Replace with a proper allocator if individual free() is needed
     */
    void free(void* ptr);
};

extern "C" {
#endif

/* Physical Memory Manager - C-compatible interface */

/**
 * @brief Initialize the physical memory manager
 * @param boot_info Boot information from bootloader (contains memory map)
 * @note This must be called early in kernel initialization, before any memory allocation
 */
void pmm_init(BootInfo* boot_info);

/**
 * @brief Allocate a single 4KB physical memory page
 * @return Physical address of allocated page, or NULL if out of memory
 * @note Returns first available page from the page bitmap
 */
void* pmm_alloc_page(void);

/**
 * @brief Free a previously allocated physical memory page
 * @param page Physical address of the page to free
 */
void pmm_free_page(void* page);

/**
 * @brief Get total amount of physical memory managed by PMM
 * @return Total memory in bytes
 */
uint64_t pmm_get_total_memory(void);

/**
 * @brief Get amount of free physical memory currently available
 * @return Free memory in bytes
 */
uint64_t pmm_get_free_memory(void);

/* Kernel Heap Allocator - C-compatible interface */

/**
 * @brief Initialize the kernel heap allocator
 * @param start Starting address of heap memory region
 * @param size Size of heap region in bytes
 * @note The memory region should be allocated from physical memory manager first
 */
void heap_init(void* start, size_t size);

/**
 * @brief Allocate memory from kernel heap (like malloc)
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL if out of heap space
 * @note Memory is 16-byte aligned
 * @note Cannot be freed individually (bump allocator limitation)
 */
void* kmalloc(size_t size);

/**
 * @brief Allocate and zero-initialize array memory from kernel heap (like calloc)
 * @param num Number of elements
 * @param size Size of each element in bytes
 * @return Pointer to allocated and zeroed memory, or NULL if out of space
 */
void* kcalloc(size_t num, size_t size);

/**
 * @brief Free kernel heap memory (no-op in current implementation)
 * @param ptr Pointer to memory to free
 * @note This function currently does nothing - bump allocator cannot free individual allocations
 */
void kfree(void* ptr);

/* Memory Utility Functions */

/**
 * @brief Fill memory with a constant byte value
 * @param dest Pointer to memory block to fill
 * @param val Value to set (converted to unsigned char)
 * @param count Number of bytes to set
 * @return Pointer to dest
 * @note Simple byte-by-byte implementation
 */
void* memset(void* dest, int val, size_t count);

/**
 * @brief Copy memory from source to destination
 * @param dest Pointer to destination buffer
 * @param src Pointer to source buffer
 * @param count Number of bytes to copy
 * @return Pointer to dest
 * @warning Memory regions must not overlap
 */
void* memcpy(void* dest, const void* src, size_t count);

/**
 * @brief Compare two memory blocks
 * @param s1 Pointer to first memory block
 * @param s2 Pointer to second memory block
 * @param count Number of bytes to compare
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int memcmp(const void* s1, const void* s2, size_t count);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_MEMORY_H
