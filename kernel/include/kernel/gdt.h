#ifndef METALOS_KERNEL_GDT_H
#define METALOS_KERNEL_GDT_H

#include <stdint.h>

/**
 * @struct gdt_entry_t
 * @brief Global Descriptor Table entry structure (8 bytes)
 * 
 * The GDT defines memory segments in protected/long mode. Each entry describes
 * a segment with base address, limit (size), and access rights. In 64-bit long mode,
 * segmentation is mostly disabled, but the GDT is still required for:
 * - Code/Data segment selectors
 * - Privilege level enforcement (Ring 0 kernel, Ring 3 user)
 * - System call/interrupt handling
 * 
 * Each GDT entry is 8 bytes with the following layout:
 * - limit_low (16 bits): Lower 16 bits of segment limit
 * - base_low (16 bits): Lower 16 bits of base address
 * - base_middle (8 bits): Middle 8 bits of base address
 * - access (8 bits): Access flags (present, privilege, type)
 * - granularity (8 bits): Upper 4 bits of limit + flags (granularity, size)
 * - base_high (8 bits): Upper 8 bits of base address
 */
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

/**
 * @struct gdt_ptr_t
 * @brief GDT pointer structure used by LGDT instruction
 * 
 * This structure is loaded into the CPU using the LGDT (Load GDT) instruction.
 * It tells the CPU where the GDT is located in memory and how large it is.
 * 
 * - limit: Size of GDT in bytes minus 1
 * - base: 64-bit linear address of the GDT
 */
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_ptr_t;

#ifdef __cplusplus
/**
 * @class GDT
 * @brief Global Descriptor Table manager for x86-64 long mode
 * 
 * The GDT (Global Descriptor Table) is a data structure used by x86 processors
 * to define memory segments. Although segmentation is mostly flat in 64-bit mode,
 * the GDT is still required for:
 * - Defining code and data segments
 * - Setting privilege levels (CPL 0 for kernel, CPL 3 for user)
 * - Enabling system calls and interrupts
 * 
 * MetalOS uses a minimal 5-entry GDT:
 * 0. Null descriptor (required by CPU, never used)
 * 1. Kernel code segment (64-bit, ring 0, executable)
 * 2. Kernel data segment (64-bit, ring 0, writable)
 * 3. User code segment (64-bit, ring 3, executable)
 * 4. User data segment (64-bit, ring 3, writable)
 * 
 * In 64-bit mode, segment bases and limits are mostly ignored, but the access
 * rights (privilege level, executable flag) are still enforced.
 */
class GDT {
private:
    gdt_entry_t entries[5]; ///< Array of 5 GDT entries
    gdt_ptr_t gdtPtr;       ///< GDT pointer for LGDT instruction
    
    /**
     * @brief Set a GDT entry with specified parameters
     * @param num Entry index (0-4)
     * @param base Base address (mostly ignored in 64-bit mode)
     * @param limit Segment limit (mostly ignored in 64-bit mode)
     * @param access Access byte (present, DPL, type flags)
     * @param gran Granularity byte (upper limit bits + flags)
     */
    void setGate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

public:
    /** @brief Constructor - initializes GDT pointer structure */
    GDT();
    
    /**
     * @brief Initialize the GDT and load it into the CPU
     * @note Sets up 5 descriptors: null, kernel code/data, user code/data
     * @note Calls gdt_flush() assembly function to load GDT using LGDT
     */
    void init();
};

extern "C" {
#endif

/**
 * @brief Initialize the Global Descriptor Table
 * @note Must be called early in kernel initialization before enabling interrupts
 */
void gdt_init(void);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_GDT_H
