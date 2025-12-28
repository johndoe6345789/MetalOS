/**
 * @file main.cpp
 * @brief MetalOS Kernel Main Entry Point
 * 
 * This is the heart of MetalOS - an extremely minimalist kernel designed to run
 * a single application (QT6 Hello World). The kernel provides only the essential
 * hardware initialization needed to run the application.
 * 
 * Design Philosophy:
 * - No scheduler: Single application, always running
 * - No process management: One process only
 * - No complex memory management: Simple bump allocator
 * - No filesystem: Application embedded in boot image
 * - Multicore support: All cores initialized for future parallel processing
 * 
 * Boot sequence:
 * 1. UEFI bootloader loads kernel and provides boot information
 * 2. Kernel initializes hardware (GDT, IDT, memory, timer, PCI, SMP)
 * 3. Kernel will eventually jump directly to the application
 * 
 * Target size: < 150 KB (achieved through extreme minimalism)
 */

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

/**
 * @brief Kernel main entry point - called by bootloader
 * 
 * This is the first C++ function executed after the bootloader transfers control.
 * It receives boot information from UEFI and performs minimal hardware initialization.
 * 
 * Initialization sequence:
 * 
 * 1. GDT (Global Descriptor Table):
 *    - Required for x86-64 segmentation and privilege levels
 *    - Sets up kernel/user code and data segments
 * 
 * 2. IDT (Interrupt Descriptor Table):
 *    - Sets up interrupt and exception handlers
 *    - Remaps PIC to avoid conflicts with CPU exceptions
 *    - Enables hardware interrupts
 * 
 * 3. Physical Memory Manager:
 *    - Initializes page bitmap for 4KB page allocation
 *    - Currently assumes 128MB at 16MB physical address
 *    - TODO: Parse UEFI memory map for proper detection
 * 
 * 4. Kernel Heap:
 *    - Allocates 1MB (256 pages) for kernel dynamic allocation
 *    - Uses simple bump allocator (no free() support)
 * 
 * 5. Timer (PIT):
 *    - Programs 8254 PIT for 1000 Hz (1ms ticks)
 *    - Used for timekeeping and delays
 * 
 * 6. PCI Bus:
 *    - Enumerates all PCI devices
 *    - Discovers GPU and other hardware
 *    - Stores device information for later use
 * 
 * 7. SMP (Multi-Processing):
 *    - Initializes Local APIC on BSP
 *    - Starts all available Application Processor cores
 *    - Currently APs idle; only BSP runs application
 * 
 * After initialization, the kernel will eventually:
 * - Initialize GPU for framebuffer graphics
 * - Set up minimal input (PS/2 keyboard/mouse or USB)
 * - Jump directly to QT6 application entry point
 * 
 * For now, it enters an infinite halt loop waiting for implementation.
 * 
 * @param boot_info Pointer to boot information structure from UEFI bootloader containing:
 *                  - Framebuffer information (base, width, height, pitch, bpp)
 *                  - Kernel location and size
 *                  - ACPI RSDP pointer
 *                  - UEFI memory map
 * 
 * @note This function should never return
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
