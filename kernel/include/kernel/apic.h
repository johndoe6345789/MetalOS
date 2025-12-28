#ifndef METALOS_KERNEL_APIC_H
#define METALOS_KERNEL_APIC_H

#include <stdint.h>
#include <stdbool.h>

// APIC register offsets
#define APIC_REG_ID           0x020
#define APIC_REG_VERSION      0x030
#define APIC_REG_TPR          0x080
#define APIC_REG_EOI          0x0B0
#define APIC_REG_SPURIOUS     0x0F0
#define APIC_REG_ICR_LOW      0x300
#define APIC_REG_ICR_HIGH     0x310
#define APIC_REG_LVT_TIMER    0x320
#define APIC_REG_LVT_ERROR    0x370

// IPI types
#define APIC_IPI_INIT         0x500
#define APIC_IPI_STARTUP      0x600

#ifdef __cplusplus
/**
 * @class APIC
 * @brief Advanced Programmable Interrupt Controller manager for multicore systems
 * 
 * The APIC (Advanced Programmable Interrupt Controller) replaces the legacy 8259 PIC
 * in modern x86-64 systems. Each CPU core has its own Local APIC that:
 * - Receives interrupts from devices and other CPUs
 * - Sends End-Of-Interrupt (EOI) signals
 * - Sends Inter-Processor Interrupts (IPIs) to other cores
 * - Manages per-core timer and performance monitoring interrupts
 * 
 * The Local APIC is memory-mapped at physical address 0xFEE00000 by default.
 * All APIC operations are performed by reading/writing 32-bit registers at
 * specific offsets from this base address.
 * 
 * Key APIC concepts:
 * - Local APIC: One per CPU core, handles that core's interrupts
 * - APIC ID: Unique identifier for each Local APIC (each CPU core)
 * - IPI (Inter-Processor Interrupt): Interrupt sent from one CPU to another
 * - EOI (End-Of-Interrupt): Signal that interrupt handling is complete
 * - ICR (Interrupt Command Register): Used to send IPIs to other cores
 */
class APIC {
private:
    volatile uint32_t* apicBase; ///< Pointer to APIC memory-mapped registers (0xFEE00000)
    
    /**
     * @brief Read a 32-bit value from an APIC register
     * @param offset Register offset from APIC base (e.g., APIC_REG_ID)
     * @return 32-bit register value
     */
    uint32_t read(uint32_t offset) const;
    
    /**
     * @brief Write a 32-bit value to an APIC register
     * @param offset Register offset from APIC base
     * @param value 32-bit value to write
     */
    void write(uint32_t offset, uint32_t value);

public:
    /** @brief Constructor - initializes APIC base address to 0xFEE00000 */
    APIC();
    
    /**
     * @brief Check if APIC is available on this CPU
     * @return true if APIC is present and can be used, false otherwise
     * @note Uses CPUID instruction to check for APIC support (bit 9 of EDX)
     */
    bool isAvailable() const;
    
    /**
     * @brief Initialize the Local APIC for this CPU core
     * @note Enables the APIC by setting bit 8 of the Spurious Interrupt Vector Register
     * @note Sets Task Priority Register to 0 to accept all interrupts
     * @note Must be called on each CPU core that will use the APIC
     */
    void init();
    
    /**
     * @brief Get the APIC ID of the current CPU core
     * @return 8-bit APIC ID (unique identifier for this core)
     * @note APIC IDs may not be sequential (e.g., 0, 2, 4, 6 on hyperthreaded systems)
     */
    uint8_t getId() const;
    
    /**
     * @brief Send End-Of-Interrupt signal to acknowledge interrupt completion
     * @note Must be called at the end of every interrupt handler that uses the APIC
     * @note Writing any value to the EOI register acknowledges the interrupt
     */
    void sendEOI();
    
    /**
     * @brief Send an Inter-Processor Interrupt to another CPU core
     * @param destApicId APIC ID of the destination CPU core
     * @param vector Interrupt vector number to deliver (0-255)
     * @param deliveryMode Delivery mode flags (e.g., APIC_IPI_INIT, APIC_IPI_STARTUP)
     * 
     * IPIs are used for:
     * - Starting application processors (APs) during SMP initialization
     * - TLB shootdowns when changing page tables
     * - Sending signals between cores
     * - Emergency core wakeup or halt
     * 
     * @note Waits for previous IPI to complete before sending new one
     */
    void sendIPI(uint8_t destApicId, uint8_t vector, uint32_t deliveryMode);
};

extern "C" {
#endif

/* APIC - C-compatible interface */

/**
 * @brief Check if Local APIC is available on this system
 * @return true if APIC is present, false otherwise
 */
bool apic_is_available(void);

/**
 * @brief Initialize the Local APIC for the current CPU core
 * @note Must be called on each core before using APIC functionality
 */
void apic_init(void);

/**
 * @brief Get APIC ID of the current CPU core
 * @return 8-bit APIC ID
 */
uint8_t apic_get_id(void);

/**
 * @brief Send End-Of-Interrupt signal
 * @note Call at end of interrupt handlers that use APIC
 */
void apic_send_eoi(void);

/**
 * @brief Send Inter-Processor Interrupt to another core
 * @param dest_apic_id Target core's APIC ID
 * @param vector Interrupt vector number
 * @param delivery_mode IPI delivery mode (APIC_IPI_INIT, APIC_IPI_STARTUP, etc.)
 */
void apic_send_ipi(uint8_t dest_apic_id, uint8_t vector, uint32_t delivery_mode);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_APIC_H
