/*
 * MetalOS Kernel - Global Descriptor Table (GDT)
 * 
 * Minimal GDT setup for x86_64 long mode
 * Only what's needed for our single-app OS
 */

#include "kernel/gdt.h"

// Load GDT (assembly)
extern "C" void gdt_flush(uint64_t);

// GDT class implementation
GDT::GDT() {
    gdtPtr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdtPtr.base = (uint64_t)&entries;
}

void GDT::setGate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    entries[num].base_low = (base & 0xFFFF);
    entries[num].base_middle = (base >> 16) & 0xFF;
    entries[num].base_high = (base >> 24) & 0xFF;
    
    entries[num].limit_low = (limit & 0xFFFF);
    entries[num].granularity = (limit >> 16) & 0x0F;
    entries[num].granularity |= gran & 0xF0;
    entries[num].access = access;
}

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
