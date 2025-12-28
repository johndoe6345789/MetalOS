/**
 * @file gdt.cpp
 * @brief Implementation of Global Descriptor Table (GDT) manager
 * 
 * The GDT is required by x86-64 processors even though segmentation is largely
 * disabled in 64-bit mode. It defines code and data segments with privilege levels.
 */

#include "kernel/gdt.h"

// Load GDT (assembly)
extern "C" void gdt_flush(uint64_t);

/* GDT class implementation */

/**
 * @brief Constructor - initializes GDT pointer structure
 * 
 * Sets up the GDTR (GDT Register) structure that will be loaded into the CPU.
 * The limit is the size of the GDT minus 1, and the base is the memory address.
 */
GDT::GDT() {
    gdtPtr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdtPtr.base = (uint64_t)&entries;
}

/**
 * @brief Set a GDT entry with specified parameters
 * 
 * Fills in all fields of a GDT entry. In 64-bit mode, the base and limit are
 * largely ignored, but the access flags (privilege level, executable) are enforced.
 * 
 * @param num Entry index (0-4)
 * @param base Base address (mostly ignored in 64-bit mode)
 * @param limit Segment limit (mostly ignored in 64-bit mode)
 * @param access Access byte containing:
 *               - Bit 7: Present (must be 1 for valid segment)
 *               - Bits 5-6: DPL (Descriptor Privilege Level): 0=kernel, 3=user
 *               - Bit 4: Descriptor type (1 for code/data)
 *               - Bit 3: Executable (1 for code, 0 for data)
 *               - Bit 1: Readable/Writable
 * @param gran Granularity byte containing:
 *             - Bit 7: Granularity (1=4KB blocks, 0=1 byte blocks)
 *             - Bit 6: Size (1=32-bit, 0=16-bit; for 64-bit use access flags)
 *             - Bit 5: Long mode (1=64-bit code segment)
 *             - Bits 0-3: Upper 4 bits of limit
 */
void GDT::setGate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    entries[num].base_low = (base & 0xFFFF);
    entries[num].base_middle = (base >> 16) & 0xFF;
    entries[num].base_high = (base >> 24) & 0xFF;
    
    entries[num].limit_low = (limit & 0xFFFF);
    entries[num].granularity = (limit >> 16) & 0x0F;
    entries[num].granularity |= gran & 0xF0;
    entries[num].access = access;
}

/**
 * @brief Initialize the GDT with required segments and load it
 * 
 * Sets up a minimal GDT with 5 entries:
 * 0. Null descriptor (required by CPU, must be all zeros)
 * 1. Kernel code segment (CPL 0, 64-bit, executable, readable)
 * 2. Kernel data segment (CPL 0, 64-bit, writable)
 * 3. User code segment (CPL 3, 64-bit, executable, readable)
 * 4. User data segment (CPL 3, 64-bit, writable)
 * 
 * After setting up the entries, calls gdt_flush() assembly function to:
 * - Load GDTR using LGDT instruction
 * - Reload segment registers with new selectors
 * 
 * Access byte values:
 * - 0x9A = 10011010 = Present, Ring 0, Code, Executable, Readable
 * - 0x92 = 10010010 = Present, Ring 0, Data, Writable
 * - 0xFA = 11111010 = Present, Ring 3, Code, Executable, Readable
 * - 0xF2 = 11110010 = Present, Ring 3, Data, Writable
 * 
 * Granularity values:
 * - 0xA0 = 10100000 = Long mode (64-bit)
 * - 0xC0 = 11000000 = 32-bit mode (for data segments in 64-bit mode)
 */
void GDT::init() {
    // Null descriptor
    setGate(0, 0, 0, 0, 0);
    
    // Kernel code segment (64-bit)
    // Access: Present, Ring 0, Code, Executable, Readable
    setGate(1, 0, 0xFFFFFFFF, 0x9A, 0xA0);
    
    // Kernel data segment (64-bit)
    // Access: Present, Ring 0, Data, Writable
    setGate(2, 0, 0xFFFFFFFF, 0x92, 0xC0);
    
    // User code segment (64-bit)
    // Access: Present, Ring 3, Code, Executable, Readable
    setGate(3, 0, 0xFFFFFFFF, 0xFA, 0xA0);
    
    // User data segment (64-bit)
    // Access: Present, Ring 3, Data, Writable
    setGate(4, 0, 0xFFFFFFFF, 0xF2, 0xC0);
    
    // Flush GDT
    gdt_flush((uint64_t)&gdtPtr);
}

// Global GDT instance
static GDT globalGDT;

// C-compatible wrapper function
extern "C" {

void gdt_init(void) {
    globalGDT.init();
}

} // extern "C"
