/*
 * MetalOS Kernel - APIC (Advanced Programmable Interrupt Controller)
 * 
 * Local APIC support for multicore systems
 * Replaces legacy PIC for per-CPU interrupt handling
 */

#include "kernel/apic.h"

// APIC base address (default, can be read from MSR)
#define APIC_BASE_MSR 0x1B

// Read CPUID to check for APIC
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

// APIC class implementation
APIC::APIC() : apicBase((volatile uint32_t*)0xFEE00000) {}

uint32_t APIC::read(uint32_t offset) const {
    return apicBase[offset / 4];
}

void APIC::write(uint32_t offset, uint32_t value) {
    apicBase[offset / 4] = value;
}

bool APIC::isAvailable() const {
    return cpuidHasAPIC();
}

void APIC::init() {
    // Enable APIC via spurious interrupt vector register
    // Set spurious vector to 0xFF and enable APIC (bit 8)
    write(APIC_REG_SPURIOUS, 0x1FF);
    
    // Set Task Priority Register to 0 (accept all interrupts)
    write(APIC_REG_TPR, 0);
}

uint8_t APIC::getId() const {
    uint32_t idReg = read(APIC_REG_ID);
    return (idReg >> 24) & 0xFF;
}

void APIC::sendEOI() {
    write(APIC_REG_EOI, 0);
}

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
