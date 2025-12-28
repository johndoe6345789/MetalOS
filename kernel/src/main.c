/*
 * MetalOS Kernel - Main Entry Point
 * 
 * EXTREME MINIMAL kernel - only what's needed for QT6 Hello World.
 * Now with basic multicore support for better performance!
 * Just: boot -> init hardware (all cores) -> run app.
 */

#include "kernel/kernel.h"
#include "kernel/gdt.h"
#include "kernel/interrupts.h"
#include "kernel/memory.h"
#include "kernel/pci.h"
#include "kernel/timer.h"
#include "kernel/smp.h"

/*
 * Kernel main entry point
 * Called by bootloader with boot information
 * 
 * Initializes all hardware including multicore support.
 * Simple design: all cores initialized but only BSP runs app.
 * Future: could distribute work across cores for better performance.
 */
void kernel_main(BootInfo* boot_info) {
    // Initialize GDT (Global Descriptor Table)
    gdt_init();
    
    // Initialize IDT (Interrupt Descriptor Table)
    idt_init();
    
    // Initialize physical memory manager
    pmm_init(boot_info);
    
    // Initialize kernel heap (allocate 256 pages = 1MB for kernel heap)
    void* heap_start_page = pmm_alloc_page();
    if (heap_start_page) {
        // Allocate additional pages for heap (256 pages total)
        for (int i = 1; i < 256; i++) {
            pmm_alloc_page();  // Allocate contiguous pages
        }
        heap_init(heap_start_page, 256 * PAGE_SIZE);  // 1MB heap
    }
    
    // Initialize timer (1000 Hz = 1ms per tick)
    timer_init(TIMER_FREQUENCY);
    
    // Initialize PCI bus
    pci_init();
    
    // Initialize SMP (Symmetric Multi-Processing)
    // This will detect and start all available CPU cores
    smp_init();
    
    // Print CPU info (if we had console, would show core count here)
    // For now, just continue - all cores are initialized
    
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
 * Simple multicore kernel. All cores initialized but only BSP runs app.
 * All cores available for future parallel processing.
 * 
 * Total kernel size target: < 150 KB (with multicore support)
 */
