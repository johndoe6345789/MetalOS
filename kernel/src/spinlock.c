/*
 * MetalOS Kernel - Spinlock
 * 
 * Simple spinlock implementation for multicore synchronization
 * Uses x86 atomic instructions
 */

#include "kernel/spinlock.h"

// Initialize spinlock
void spinlock_init(spinlock_t* lock) {
    lock->lock = 0;
}

// Acquire spinlock (blocking)
void spinlock_acquire(spinlock_t* lock) {
    while (1) {
        // Try to acquire lock using atomic exchange
        uint32_t old_value;
        __asm__ volatile(
            "xchgl %0, %1"
            : "=r"(old_value), "+m"(lock->lock)
            : "0"(1)
            : "memory"
        );
        
        // If old value was 0, we got the lock
        if (old_value == 0) {
            return;
        }
        
        // Spin with pause instruction to improve performance
        __asm__ volatile("pause" ::: "memory");
    }
}

// Try to acquire spinlock (non-blocking)
bool spinlock_try_acquire(spinlock_t* lock) {
    uint32_t old_value;
    __asm__ volatile(
        "xchgl %0, %1"
        : "=r"(old_value), "+m"(lock->lock)
        : "0"(1)
        : "memory"
    );
    
    return (old_value == 0);
}

// Release spinlock
void spinlock_release(spinlock_t* lock) {
    // Memory barrier to ensure all previous stores are visible
    __asm__ volatile("" ::: "memory");
    
    // Release the lock
    lock->lock = 0;
}

// Check if locked
bool spinlock_is_locked(spinlock_t* lock) {
    return lock->lock != 0;
}
