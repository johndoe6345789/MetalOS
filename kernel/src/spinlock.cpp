/*
 * MetalOS Kernel - Spinlock
 * 
 * Simple spinlock implementation for multicore synchronization
 * Uses x86 atomic instructions
 */

#include "kernel/spinlock.h"

// Spinlock class implementation

Spinlock::Spinlock() : lock(0) {}

void Spinlock::init() {
    lock = 0;
}

void Spinlock::acquire() {
    while (1) {
        // Try to acquire lock using atomic exchange
        uint32_t old_value;
        __asm__ volatile(
            "xchgl %0, %1"
            : "=r"(old_value), "+m"(lock)
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

bool Spinlock::tryAcquire() {
    uint32_t old_value;
    __asm__ volatile(
        "xchgl %0, %1"
        : "=r"(old_value), "+m"(lock)
        : "0"(1)
        : "memory"
    );
    
    return (old_value == 0);
}

void Spinlock::release() {
    // Memory barrier to ensure all previous stores are visible
    __asm__ volatile("" ::: "memory");
    
    // Release the lock
    lock = 0;
}

bool Spinlock::isLocked() const {
    return lock != 0;
}

// C-compatible wrapper functions for backward compatibility
extern "C" {

void spinlock_init(spinlock_t* lock) {
    lock->lock = 0;
}

void spinlock_acquire(spinlock_t* lock) {
    while (1) {
        uint32_t old_value;
        __asm__ volatile(
            "xchgl %0, %1"
            : "=r"(old_value), "+m"(lock->lock)
            : "0"(1)
            : "memory"
        );
        
        if (old_value == 0) {
            return;
        }
        
        __asm__ volatile("pause" ::: "memory");
    }
}

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

void spinlock_release(spinlock_t* lock) {
    __asm__ volatile("" ::: "memory");
    lock->lock = 0;
}

bool spinlock_is_locked(spinlock_t* lock) {
    return lock->lock != 0;
}

} // extern "C"
