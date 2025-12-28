/*
 * MetalOS Kernel - Main Entry Point
 * 
 * Minimal kernel implementation for running QT6 applications.
 * Only implements what's absolutely necessary.
 */

#include "kernel/kernel.h"
#include "kernel/console.h"

/*
 * Kernel main entry point
 * Called by bootloader with boot information
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
    console_print("MetalOS Kernel v0.1.0\n");
    console_print("=====================\n\n");
    
    console_print("Initializing minimal kernel...\n");
    
    // TODO: Initialize memory management
    console_print("[ ] Memory management\n");
    
    // TODO: Initialize interrupt handling
    console_print("[ ] Interrupt handling\n");
    
    // TODO: Initialize PCI bus
    console_print("[ ] PCI enumeration\n");
    
    // TODO: Initialize GPU driver
    console_print("[ ] GPU driver (Radeon RX 6600)\n");
    
    // TODO: Initialize input devices
    console_print("[ ] Input devices\n");
    
    // TODO: Setup system calls
    console_print("[ ] System call interface\n");
    
    // TODO: Load and run QT6 hello world application directly (no shell/init)
    console_print("[ ] Loading QT6 Hello World application...\n");
    
    console_print("\nKernel initialization complete.\n");
    console_print("Booting directly into application (no shell)...\n");
    
    // TODO: Jump to QT6 hello world application entry point
    // The application will run full-screen until exit
    // No shell, no command line - just the one app
    
    // Halt - in real implementation, we'd start the application
    while(1) {
        __asm__ volatile("hlt");
    }
}
