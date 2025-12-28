#ifndef METALOS_KERNEL_TIMER_H
#define METALOS_KERNEL_TIMER_H

#include <stdint.h>

// Timer frequency (Hz)
#define TIMER_FREQUENCY 1000  // 1ms per tick

// Timer functions
void timer_init(uint32_t frequency);
uint64_t timer_get_ticks(void);
void timer_wait(uint32_t ticks);

// Timer interrupt handler (called from ISR)
void timer_handler(void);

#endif // METALOS_KERNEL_TIMER_H
