/**
 * @file apic.cpp
 * @brief Implementation of APIC (Advanced Programmable Interrupt Controller) manager
 * 
 * The Local APIC is a key component of modern x86-64 multicore systems. It replaces
 * the legacy 8259 PIC and provides per-CPU interrupt handling capabilities.
 */

#include "kernel/apic.h"

// APIC base address (default, can be read from MSR)
#define APIC_BASE_MSR 0x1B

/**
 * @brief Check if CPU has APIC support using CPUID instruction
 * 
 * The CPUID instruction provides information about CPU features. Function 1
 * returns feature flags in the EDX register, where bit 9 indicates APIC support.
 * 
 * @return true if APIC is supported, false otherwise
 */
static bool cpuidHasAPIC(void) {
    uint32_t eax, ebx, ecx, edx;
    
    // CPUID function 1
    __asm__ volatile(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
    );
    
    // APIC is bit 9 of EDX
    return (edx & (1 << 9)) != 0;
}

/* APIC class implementation */

/**
 * @brief Constructor - sets APIC base address to default memory-mapped location
 * 
 * The Local APIC registers are accessed through memory-mapped I/O at physical
 * address 0xFEE00000 by default. This can be changed via the IA32_APIC_BASE MSR,
 * but we use the default location for simplicity.
 */
APIC::APIC() : apicBase((volatile uint32_t*)0xFEE00000) {}

/**
 * @brief Read a 32-bit value from an APIC register
 * 
 * APIC registers are 32 bits wide and located at 16-byte aligned offsets.
 * The apicBase pointer is to uint32_t, so we divide the offset by 4 to get
 * the array index.
 * 
 * @param offset Register offset in bytes (e.g., 0x020 for APIC ID register)
 * @return 32-bit register value
 */
uint32_t APIC::read(uint32_t offset) const {
    return apicBase[offset / 4];
}

/**
 * @brief Write a 32-bit value to an APIC register
 * 
 * @param offset Register offset in bytes
 * @param value 32-bit value to write
 */
void APIC::write(uint32_t offset, uint32_t value) {
    apicBase[offset / 4] = value;
}

bool APIC::isAvailable() const {
    return cpuidHasAPIC();
}

/**
 * @brief Initialize the Local APIC for this CPU core
 * 
 * This function:
 * 1. Enables the APIC by setting the software enable bit (bit 8) in the
 *    Spurious Interrupt Vector Register
 * 2. Sets the spurious vector to 0xFF (unused vector for spurious interrupts)
 * 3. Sets Task Priority Register to 0 to accept all interrupt priorities
 * 
 * After this initialization, the APIC is ready to receive and send interrupts.
 */
void APIC::init() {
    // Enable APIC via spurious interrupt vector register
    // Set spurious vector to 0xFF and enable APIC (bit 8)
    write(APIC_REG_SPURIOUS, 0x1FF);
    
    // Set Task Priority Register to 0 (accept all interrupts)
    write(APIC_REG_TPR, 0);
}

/**
 * @brief Get the APIC ID of the current CPU core
 * 
 * The APIC ID is stored in bits 24-31 of the APIC ID register. This is a
 * unique identifier for each Local APIC (and thus each CPU core).
 * 
 * @return 8-bit APIC ID
 * @note APIC IDs may not be sequential (e.g., 0, 2, 4, 6 on hyperthreaded systems)
 */
uint8_t APIC::getId() const {
    uint32_t idReg = read(APIC_REG_ID);
    return (idReg >> 24) & 0xFF;
}

/**
 * @brief Send End-Of-Interrupt signal to acknowledge interrupt completion
 * 
 * After handling an interrupt that came through the APIC, the interrupt handler
 * must send an EOI to inform the APIC that the interrupt has been processed.
 * This allows the APIC to deliver the next interrupt if one is pending.
 * 
 * Writing any value (typically 0) to the EOI register sends the EOI.
 */
void APIC::sendEOI() {
    write(APIC_REG_EOI, 0);
}

/**
 * @brief Send an Inter-Processor Interrupt (IPI) to another CPU core
 * 
 * IPIs are used for:
 * - Starting Application Processors (APs) during SMP initialization (INIT + SIPI)
 * - Sending signals or notifications to other cores
 * - TLB shootdowns when changing page tables
 * - Requesting other cores to perform specific actions
 * 
 * The IPI is sent using the Interrupt Command Register (ICR), which consists of
 * two 32-bit registers (high and low). The high register contains the destination
 * APIC ID, and the low register contains the delivery mode, vector, and control flags.
 * 
 * @param destApicId APIC ID of the destination CPU core
 * @param vector Interrupt vector number (or page number for SIPI)
 * @param deliveryMode Delivery mode (INIT, SIPI, fixed, etc.)
 * 
 * @note This function waits for any pending IPI to complete before sending a new one
 */
void APIC::sendIPI(uint8_t destApicId, uint8_t vector, uint32_t deliveryMode) {
    // Wait for previous IPI to complete
    while (read(APIC_REG_ICR_LOW) & (1 << 12)) {
        __asm__ volatile("pause");
    }
    
    // Set destination in high register
    write(APIC_REG_ICR_HIGH, ((uint32_t)destApicId) << 24);
    
    // Send IPI with delivery mode and vector in low register
    write(APIC_REG_ICR_LOW, deliveryMode | vector);
}

// Global APIC instance
static APIC globalAPIC;

// C-compatible wrapper functions
extern "C" {

bool apic_is_available(void) {
    return globalAPIC.isAvailable();
}

void apic_init(void) {
    globalAPIC.init();
}

uint8_t apic_get_id(void) {
    return globalAPIC.getId();
}

void apic_send_eoi(void) {
    globalAPIC.sendEOI();
}

void apic_send_ipi(uint8_t dest_apic_id, uint8_t vector, uint32_t delivery_mode) {
    globalAPIC.sendIPI(dest_apic_id, vector, delivery_mode);
}

} // extern "C"
