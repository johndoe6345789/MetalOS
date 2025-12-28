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
#define PIC1_DATA    0x21

// PIT constants
#define PIT_BASE_FREQUENCY 1193182  // Hz

// I/O port access functions
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Timer class implementation
Timer::Timer() : ticks(0) {}

void Timer::init(uint32_t frequency) {
    // Calculate divisor
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    
    // Send command byte: channel 0, rate generator, lo/hi byte
    outb(PIT_COMMAND, 0x36);
    
    // Send divisor
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
    
    // Enable timer interrupt (IRQ0)
    // Unmask IRQ0 in PIC
    uint8_t mask = inb(PIC1_DATA);
    mask &= ~0x01;  // Clear bit 0 (IRQ0)
    outb(PIC1_DATA, mask);
    
    ticks = 0;
}

uint64_t Timer::getTicks() const {
    return ticks;
}

void Timer::wait(uint32_t waitTicks) const {
    uint64_t target = ticks + waitTicks;
    while (ticks < target) {
        __asm__ volatile("hlt");
    }
}

void Timer::handleInterrupt() {
    ticks++;
}

// Global timer instance
static Timer globalTimer;

// C-compatible wrapper functions
extern "C" {

void timer_init(uint32_t frequency) {
    globalTimer.init(frequency);
}

uint64_t timer_get_ticks(void) {
    return globalTimer.getTicks();
}

void timer_wait(uint32_t ticks) {
    globalTimer.wait(ticks);
}

void timer_handler(void) {
    globalTimer.handleInterrupt();
}

} // extern "C"
