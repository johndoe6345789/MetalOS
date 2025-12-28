/*
 * MetalOS Kernel - Global Descriptor Table (GDT)
 * 
 * Minimal GDT setup for x86_64 long mode
 * Only what's needed for our single-app OS
 */

#include "kernel/gdt.h"

// GDT entries (minimal for x86_64)
// In long mode, most segmentation is ignored, but we still need a valid GDT
static gdt_entry_t gdt[5];
static gdt_ptr_t gdt_ptr;

// Set a GDT entry
static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access = access;
}

// Load GDT (assembly)
extern void gdt_flush(uint64_t);

// Initialize GDT
void gdt_init(void) {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = (uint64_t)&gdt;
    
    // Null descriptor
    gdt_set_gate(0, 0, 0, 0, 0);
    
    // Kernel code segment (64-bit)
    // Access: Present, Ring 0, Code, Executable, Readable
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xA0);
    
    // Kernel data segment (64-bit)
    // Access: Present, Ring 0, Data, Writable
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xC0);
    
    // User code segment (64-bit)
    // Access: Present, Ring 3, Code, Executable, Readable
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xA0);
    
    // User data segment (64-bit)
    // Access: Present, Ring 3, Data, Writable
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xC0);
    
    // Flush GDT
    gdt_flush((uint64_t)&gdt_ptr);
}
