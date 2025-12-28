/*
 * MetalOS Kernel - Timer Support
 * 
 * Simple PIT (Programmable Interval Timer) support
 * Used for scheduling and timing
 */

#include "kernel/timer.h"

// PIT I/O ports
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

// PIT constants
#define PIT_BASE_FREQUENCY 1193182  // Hz

// Tick counter
static volatile uint64_t timer_ticks = 0;

// I/O port access
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Initialize timer
void timer_init(uint32_t frequency) {
    // Calculate divisor
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    
    // Send command byte: channel 0, rate generator, lo/hi byte
    outb(PIT_COMMAND, 0x36);
    
    // Send divisor
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
    
    // Enable timer interrupt (IRQ0)
    // Unmask IRQ0 in PIC
    uint8_t mask;
    __asm__ volatile("inb $0x21, %0" : "=a"(mask));
    mask &= ~0x01;  // Clear bit 0 (IRQ0)
    outb(0x21, mask);
}

// Get current tick count
uint64_t timer_get_ticks(void) {
    return timer_ticks;
}

// Wait for specified number of ticks
void timer_wait(uint32_t ticks) {
    uint64_t target = timer_ticks + ticks;
    while (timer_ticks < target) {
        __asm__ volatile("hlt");
    }
}

// Timer interrupt handler
void timer_handler(void) {
    timer_ticks++;
}
