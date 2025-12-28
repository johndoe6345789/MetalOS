#ifndef METALOS_KERNEL_SPINLOCK_H
#define METALOS_KERNEL_SPINLOCK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
/**
 * @class Spinlock
 * @brief Simple spinlock for multicore synchronization
 * 
 * A spinlock is a synchronization primitive used to protect shared data in
 * multicore systems. Unlike a mutex that blocks (sleeps), a spinlock "spins"
 * in a tight loop waiting for the lock to become available.
 * 
 * Spinlocks are appropriate when:
 * - Critical sections are very short (a few instructions)
 * - You're in interrupt context (can't sleep)
 * - Lock contention is rare
 * 
 * Spinlocks should NOT be used when:
 * - Critical section is long (wastes CPU cycles)
 * - Lock might be held for unpredictable time
 * - You can use a sleeping lock instead
 * 
 * Implementation uses x86 XCHG instruction which is:
 * - Atomic (indivisible operation)
 * - Automatically locked (works across multiple cores)
 * - Sequentially consistent (no memory reordering issues)
 * 
 * The PAUSE instruction in the spin loop:
 * - Reduces power consumption while spinning
 * - Improves performance on hyperthreaded CPUs
 * - Signals to CPU that we're in a spin-wait loop
 */
class Spinlock {
private:
    volatile uint32_t lock;  ///< Lock state: 0 = unlocked, 1 = locked

public:
    /** @brief Constructor - initializes lock to unlocked state */
    Spinlock();
    
    /**
     * @brief Initialize spinlock (set to unlocked)
     * @note Can be used to re-initialize an existing spinlock
     */
    void init();
    
    /**
     * @brief Acquire the spinlock, spinning until available
     * @note Will loop indefinitely if lock is never released
     * @note Disables interrupts on this CPU while spinning (not implemented here)
     */
    void acquire();
    
    /**
     * @brief Try to acquire spinlock without blocking
     * @return true if lock was acquired, false if already locked
     * @note Returns immediately without spinning
     */
    bool tryAcquire();
    
    /**
     * @brief Release the spinlock
     * @note Must only be called by the CPU that acquired the lock
     * @note Includes memory barrier to ensure all writes are visible
     */
    void release();
    
    /**
     * @brief Check if spinlock is currently locked
     * @return true if locked, false if unlocked
     * @note Result may be stale immediately after checking
     */
    bool isLocked() const;
};

extern "C" {
#endif

/**
 * @struct spinlock_t
 * @brief C-compatible spinlock structure
 */
typedef struct {
    volatile uint32_t lock;  ///< Lock state: 0 = unlocked, 1 = locked
} spinlock_t;

/* Spinlock - C-compatible interface */

/**
 * @brief Initialize a spinlock
 * @param lock Pointer to spinlock structure
 */
void spinlock_init(spinlock_t* lock);

/**
 * @brief Acquire a spinlock (spin until available)
 * @param lock Pointer to spinlock structure
 */
void spinlock_acquire(spinlock_t* lock);

/**
 * @brief Try to acquire spinlock without blocking
 * @param lock Pointer to spinlock structure
 * @return true if acquired, false if already locked
 */
bool spinlock_try_acquire(spinlock_t* lock);

/**
 * @brief Release a spinlock
 * @param lock Pointer to spinlock structure
 */
void spinlock_release(spinlock_t* lock);

/**
 * @brief Check if spinlock is locked
 * @param lock Pointer to spinlock structure
 * @return true if locked, false if unlocked
 */
bool spinlock_is_locked(spinlock_t* lock);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_SPINLOCK_H
