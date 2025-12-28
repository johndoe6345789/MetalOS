/*
 * MetalOS Kernel - Main Entry Point
 * 
 * EXTREME MINIMAL kernel - only what's needed for QT6 Hello World.
 * No scheduler, no process management, no filesystem, no nothing.
 * Just: boot -> init GPU -> init input -> run app.
 */

#include "kernel/kernel.h"

/*
 * Kernel main entry point
 * Called by bootloader with boot information
 * 
 * This is it. The entire OS. No scheduler, no processes, no filesystem.
 * Just set up hardware and jump to the QT6 app.
 */
void kernel_main(BootInfo* boot_info) {
    // Suppress unused parameter warning
    (void)boot_info;
    
    // TODO: Set up minimal page tables (identity mapped or simple offset)
    
    // TODO: Set up IDT with only interrupts we need:
    //       - Keyboard/mouse (USB or PS/2)
    //       - Timer (for QT event loop)
    //       - GPU (if needed)
    //       That's it! Maybe 5 interrupt handlers total.
    
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
