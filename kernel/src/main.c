/*
 * MetalOS Kernel - Main Entry Point
 * 
 * EXTREME MINIMAL kernel - only what's needed for QT6 Hello World.
 * No scheduler, no process management, no filesystem, no nothing.
 * Just: boot -> init GPU -> init input -> run app.
 */

#include "kernel/kernel.h"
#include "kernel/gdt.h"
#include "kernel/interrupts.h"
#include "kernel/memory.h"
#include "kernel/pci.h"
#include "kernel/timer.h"

/*
 * Kernel main entry point
 * Called by bootloader with boot information
 * 
 * This is it. The entire OS. No scheduler, no processes, no filesystem.
 * Just set up hardware and jump to the QT6 app.
 */
void kernel_main(BootInfo* boot_info) {
    // Initialize GDT (Global Descriptor Table)
    gdt_init();
    
    // Initialize IDT (Interrupt Descriptor Table)
    idt_init();
    
    // Initialize physical memory manager
    pmm_init(boot_info);
    
    // Initialize kernel heap (allocate 1MB for kernel heap)
    void* heap_mem = pmm_alloc_page();
    if (heap_mem) {
        heap_init(heap_mem, 256 * PAGE_SIZE);  // 1MB heap
    }
    
    // Initialize timer (1000 Hz = 1ms per tick)
    timer_init(TIMER_FREQUENCY);
    
    // Initialize PCI bus
    pci_init();
    
    // TODO: Set up minimal page tables (identity mapped or simple offset)
    
    // TODO: Simple memory allocator (bump allocator is fine)
    
    // TODO: Find RX 6600 GPU via PCI (hardcode vendor/device ID)
    
    // TODO: Initialize GPU - minimal
    //       - Enable BAR
    //       - Init display pipeline
    //       - Set up framebuffer at 1920x1080 (hardcoded)
    
    // TODO: Initialize input
    //       Try PS/2 first (simpler!)
    //       Fall back to minimal USB XHCI if needed
    
    // TODO: Jump directly to QT6 Hello World app
    //       No shell, no init, no fork/exec
    //       Just: jump to application entry point
    
    // TODO: Replace this with jump to QT6 app
    // For now, halt
    
    while(1) {
        __asm__ volatile("hlt");
    }
}

/*
 * That's the entire kernel. No scheduler. No processes. No filesystem.
 * Just boot, initialize hardware, run app.
 * 
 * Total kernel size target: < 100 KB
 */
