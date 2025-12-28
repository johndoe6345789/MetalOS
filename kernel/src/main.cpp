/*
 * MetalOS Kernel - Main Entry Point
 * 
 * EXTREME MINIMAL kernel - only what's needed for QT6 Hello World.
 * Now with basic multicore support for better performance!
 * Just: boot -> init hardware (all cores) -> run app.
 * 
 * C++ implementation: Root level function hands off to classes
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
 * This is the root-level function that hands off to C++ classes
 * for hardware initialization and system management.
 */
extern "C" void kernel_main(BootInfo* boot_info) {
    // Initialize GDT (Global Descriptor Table) - using GDT class
    gdt_init();
    
    // Initialize IDT (Interrupt Descriptor Table) - using InterruptManager class
    idt_init();
    
    // Initialize physical memory manager - using PhysicalMemoryManager class
    pmm_init(boot_info);
    
    // Initialize kernel heap (allocate 256 pages = 1MB for kernel heap)
    // Using HeapAllocator class
    void* heap_start_page = pmm_alloc_page();
    if (heap_start_page) {
        // Allocate additional pages for heap (256 pages total)
        for (int i = 1; i < 256; i++) {
            pmm_alloc_page();  // Allocate contiguous pages
        }
        heap_init(heap_start_page, 256 * PAGE_SIZE);  // 1MB heap
    }
    
    // Initialize timer (1000 Hz = 1ms per tick) - using Timer class
    timer_init(TIMER_FREQUENCY);
    
    // Initialize PCI bus - using PCIManager class
    pci_init();
    
    // Initialize SMP (Symmetric Multi-Processing) - using SMPManager class
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
 * 
 * C++ Design:
 * - Root-level kernel_main() function serves as entry point
 * - All subsystems implemented as classes (GDT, InterruptManager, Timer, etc.)
 * - Classes encapsulate state and behavior
 * - C-compatible wrappers maintain compatibility with assembly and bootloader
 */
