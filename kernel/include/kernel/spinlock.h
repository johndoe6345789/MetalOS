#ifndef METALOS_KERNEL_SPINLOCK_H
#define METALOS_KERNEL_SPINLOCK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
// C++ Spinlock class
class Spinlock {
private:
    volatile uint32_t lock;

public:
    Spinlock();
    
    void init();
    void acquire();
    bool tryAcquire();
    void release();
    bool isLocked() const;
};

extern "C" {
#endif

// C-compatible spinlock structure
typedef struct {
    volatile uint32_t lock;
} spinlock_t;

// C-compatible functions
void spinlock_init(spinlock_t* lock);
void spinlock_acquire(spinlock_t* lock);
bool spinlock_try_acquire(spinlock_t* lock);
void spinlock_release(spinlock_t* lock);
bool spinlock_is_locked(spinlock_t* lock);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_SPINLOCK_H
