#ifndef METALOS_KERNEL_TIMER_H
#define METALOS_KERNEL_TIMER_H

#include <stdint.h>

// Timer frequency (Hz)
#define TIMER_FREQUENCY 1000  // 1ms per tick

#ifdef __cplusplus
// C++ Timer class
class Timer {
private:
    volatile uint64_t ticks;

public:
    Timer();
    
    void init(uint32_t frequency);
    uint64_t getTicks() const;
    void wait(uint32_t waitTicks) const;
    void handleInterrupt();
};

extern "C" {
#endif

// C-compatible timer functions
void timer_init(uint32_t frequency);
uint64_t timer_get_ticks(void);
void timer_wait(uint32_t ticks);
void timer_handler(void);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_TIMER_H
