#ifndef METALOS_KERNEL_TIMER_H
#define METALOS_KERNEL_TIMER_H

#include <stdint.h>

// Timer frequency (Hz)
#define TIMER_FREQUENCY 1000  // 1ms per tick

#ifdef __cplusplus
/**
 * @class Timer
 * @brief Programmable Interval Timer (PIT) manager for system timing
 * 
 * The PIT (Programmable Interval Timer), also known as the 8253/8254 chip,
 * generates periodic timer interrupts at a configurable frequency. It's used for:
 * - System timekeeping (tracking elapsed time)
 * - Scheduling and preemption (in multi-tasking systems)
 * - Delays and timeouts
 * 
 * The PIT has a base frequency of 1.193182 MHz. By programming a divisor,
 * we can generate interrupts at lower frequencies. For example:
 * - Divisor 1193 → ~1000 Hz (1ms per tick) ← MetalOS uses this
 * - Divisor 119318 → ~10 Hz (100ms per tick)
 * 
 * Timer interrupts are delivered as IRQ0, which is mapped to interrupt vector 32
 * after the PIC is remapped.
 * 
 * @note The PIT is legacy hardware but still commonly available. Modern systems
 *       may use HPET (High Precision Event Timer) or APIC timer instead.
 */
class Timer {
private:
    volatile uint64_t ticks; ///< Number of timer interrupts received since initialization

public:
    /** @brief Constructor - initializes tick counter to 0 */
    Timer();
    
    /**
     * @brief Initialize the PIT with specified interrupt frequency
     * @param frequency Desired interrupt frequency in Hz (e.g., 1000 for 1ms ticks)
     * @note Calculates divisor = 1193182 / frequency and programs the PIT
     * @note Unmasks IRQ0 in the PIC to enable timer interrupts
     */
    void init(uint32_t frequency);
    
    /**
     * @brief Get the current tick count
     * @return Number of timer ticks since initialization
     * @note At 1000 Hz, each tick represents 1 millisecond
     */
    uint64_t getTicks() const;
    
    /**
     * @brief Wait for a specified number of timer ticks
     * @param waitTicks Number of ticks to wait
     * @note Uses HLT instruction to save power while waiting
     * @note Blocking wait - CPU will be idle during this time
     */
    void wait(uint32_t waitTicks) const;
    
    /**
     * @brief Handle timer interrupt (called from IRQ0 handler)
     * @note Increments the tick counter
     * @note Should be called from interrupt context only
     */
    void handleInterrupt();
};

extern "C" {
#endif

/* Timer - C-compatible interface */

/**
 * @brief Initialize the Programmable Interval Timer
 * @param frequency Interrupt frequency in Hz (typically 1000 for 1ms ticks)
 */
void timer_init(uint32_t frequency);

/**
 * @brief Get current timer tick count
 * @return Number of ticks since timer initialization
 */
uint64_t timer_get_ticks(void);

/**
 * @brief Wait for specified number of timer ticks
 * @param ticks Number of ticks to wait
 */
void timer_wait(uint32_t ticks);

/**
 * @brief Timer interrupt handler (called from IRQ0)
 * @note Should only be called from interrupt context
 */
void timer_handler(void);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_TIMER_H
