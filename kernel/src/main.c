/*
 * MetalOS Kernel - Main Entry Point
 * 
 * EXTREME MINIMAL kernel - only what's needed for QT6 Hello World.
 * No scheduler, no process management, no filesystem, no nothing.
 * Just: boot -> init GPU -> init input -> run app.
 */

#include "kernel/kernel.h"
#include "kernel/console.h"

/*
 * Kernel main entry point
 * Called by bootloader with boot information
 * 
 * This is it. The entire OS. No scheduler, no processes, no filesystem.
 * Just set up hardware and jump to the QT6 app.
 */
void kernel_main(BootInfo* boot_info) {
    // Initialize basic console output using framebuffer
    console_init(
        (uint32_t*)boot_info->framebuffer_base,
        boot_info->framebuffer_width,
        boot_info->framebuffer_height,
        boot_info->framebuffer_pitch
    );
    
    console_clear();
    console_print("MetalOS v0.1 - MINIMAL\n");
    console_print("======================\n\n");
    
    // TODO: Set up minimal page tables (identity mapped or simple offset)
    console_print("[ ] Memory (identity map)\n");
    
    // TODO: Set up IDT with only interrupts we need:
    //       - Keyboard/mouse (USB or PS/2)
    //       - Timer (for QT event loop)
    //       - GPU (if needed)
    //       That's it! Maybe 5 interrupt handlers total.
    console_print("[ ] Interrupts (minimal)\n");
    
    // TODO: Simple memory allocator (bump allocator is fine)
    console_print("[ ] Heap (bump allocator)\n");
    
    // TODO: Find RX 6600 GPU via PCI (hardcode vendor/device ID)
    console_print("[ ] PCI (find GPU only)\n");
    
    // TODO: Initialize GPU - minimal
    //       - Enable BAR
    //       - Init display pipeline
    //       - Set up framebuffer at 1920x1080 (hardcoded)
    console_print("[ ] GPU (RX 6600, 1920x1080)\n");
    
    // TODO: Initialize input
    //       Try PS/2 first (simpler!)
    //       Fall back to minimal USB XHCI if needed
    console_print("[ ] Input (PS/2 or USB)\n");
    
    // TODO: Jump directly to QT6 Hello World app
    //       No shell, no init, no fork/exec
    //       Just: jump to application entry point
    console_print("[ ] Jump to QT6 app\n");
    
    console_print("\nBooting app...\n");
    
    // TODO: Replace this with jump to QT6 app
    // For now, halt
    console_print("ERROR: App not linked yet\n");
    
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
