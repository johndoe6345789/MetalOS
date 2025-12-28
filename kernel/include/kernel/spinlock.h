#ifndef METALOS_KERNEL_SPINLOCK_H
#define METALOS_KERNEL_SPINLOCK_H

#include <stdint.h>
#include <stdbool.h>

// Spinlock structure
typedef struct {
    volatile uint32_t lock;
} spinlock_t;

// Initialize spinlock
void spinlock_init(spinlock_t* lock);

// Acquire spinlock
void spinlock_acquire(spinlock_t* lock);

// Try to acquire spinlock (non-blocking)
bool spinlock_try_acquire(spinlock_t* lock);

// Release spinlock
void spinlock_release(spinlock_t* lock);

// Check if locked
bool spinlock_is_locked(spinlock_t* lock);

#endif // METALOS_KERNEL_SPINLOCK_H
