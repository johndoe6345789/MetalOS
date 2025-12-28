/**
 * @file timer.cpp
 * @brief Implementation of PIT (Programmable Interval Timer) manager
 * 
 * The PIT generates periodic timer interrupts for system timekeeping and delays.
 */

#include "kernel/timer.h"

// PIT I/O ports
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIC1_DATA    0x21

// PIT constants
#define PIT_BASE_FREQUENCY 1193182  // Hz

/**
 * @brief Write a byte to an I/O port
 * 
 * Uses the x86 OUT instruction to write a byte to a hardware I/O port.
 * 
 * @param port I/O port address
 * @param value Byte value to write
 */
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

/**
 * @brief Read a byte from an I/O port
 * 
 * Uses the x86 IN instruction to read a byte from a hardware I/O port.
 * 
 * @param port I/O port address
 * @return Byte value read from port
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* Timer class implementation */

/**
 * @brief Constructor - initializes tick counter to zero
 */
Timer::Timer() : ticks(0) {}

/**
 * @brief Initialize the PIT to generate interrupts at specified frequency
 * 
 * The PIT works by counting down from a divisor value at its base frequency
 * of 1.193182 MHz. When the counter reaches zero, it generates an interrupt
 * and reloads the divisor.
 * 
 * For example, to get 1000 Hz (1ms ticks):
 * divisor = 1193182 / 1000 = 1193
 * 
 * The process:
 * 1. Calculate divisor from desired frequency
 * 2. Send command byte to configure channel 0 in rate generator mode
 * 3. Send low byte of divisor
 * 4. Send high byte of divisor
 * 5. Unmask IRQ0 in the PIC to enable timer interrupts
 * 
 * Command byte 0x36 means:
 * - Channel 0
 * - Access mode: lobyte/hibyte
 * - Mode 3: Square wave generator (rate generator)
 * - Binary counter (not BCD)
 * 
 * @param frequency Desired interrupt frequency in Hz (e.g., 1000 for 1ms ticks)
 */
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

/**
 * @brief Wait for a specified number of timer ticks
 * 
 * Calculates target tick count and uses HLT instruction to wait efficiently.
 * HLT puts the CPU in a low-power state until the next interrupt arrives.
 * 
 * @param waitTicks Number of ticks to wait
 * @note Blocking function - CPU will be idle during wait
 * @note At 1000 Hz, each tick is 1 millisecond
 */
void Timer::wait(uint32_t waitTicks) const {
    uint64_t target = ticks + waitTicks;
    while (ticks < target) {
        __asm__ volatile("hlt");
    }
}

/**
 * @brief Handle timer interrupt (increment tick counter)
 * 
 * This function is called from the IRQ0 interrupt handler every time
 * the PIT generates an interrupt. It simply increments the tick counter.
 * 
 * @note Must be called from interrupt context only
 */
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
