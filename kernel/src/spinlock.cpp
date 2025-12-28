/**
 * @file spinlock.cpp
 * @brief Implementation of spinlock synchronization primitive
 * 
 * Spinlocks provide mutual exclusion in multicore systems using atomic operations.
 */

#include "kernel/spinlock.h"

/* Spinlock class implementation */

/**
 * @brief Constructor - initializes lock to unlocked state (0)
 */
Spinlock::Spinlock() : lock(0) {}

void Spinlock::init() {
    lock = 0;
}

/**
 * @brief Acquire the spinlock (block until available)
 * 
 * This function uses the x86 XCHG (exchange) instruction, which is:
 * - Atomic: The operation cannot be interrupted midway
 * - Implicitly locked: Works correctly across multiple CPU cores
 * - Sequentially consistent: No memory reordering issues
 * 
 * The algorithm:
 * 1. Atomically exchange the lock variable with 1
 * 2. If the old value was 0, we got the lock (return)
 * 3. If the old value was 1, lock was already held (spin)
 * 4. Use PAUSE instruction while spinning to improve performance
 * 
 * The PAUSE instruction:
 * - Improves performance on hyperthreaded CPUs
 * - Reduces power consumption during spin-wait
 * - Prevents memory order violations in the spin loop
 */
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

/**
 * @brief Try to acquire the spinlock without blocking
 * 
 * Similar to acquire(), but returns immediately if lock is already held.
 * Useful when you want to try acquiring a lock but have alternative work
 * to do if it's not available.
 * 
 * @return true if lock was successfully acquired, false if already locked
 */
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

/**
 * @brief Release the spinlock
 * 
 * Simply sets the lock variable back to 0 (unlocked). The empty inline assembly
 * with "memory" clobber acts as a compiler memory barrier, ensuring all previous
 * stores are completed before the lock is released.
 * 
 * This prevents the compiler from reordering memory operations across the lock
 * boundary, which would violate the mutual exclusion guarantee.
 * 
 * @note Must only be called by the CPU that currently holds the lock
 */
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
