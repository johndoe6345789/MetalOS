/*
 * MetalOS Kernel - APIC (Advanced Programmable Interrupt Controller)
 * 
 * Local APIC support for multicore systems
 * Replaces legacy PIC for per-CPU interrupt handling
 */

#include "kernel/apic.h"

// APIC base address (default, can be read from MSR)
#define APIC_BASE_MSR 0x1B
static volatile uint32_t* apic_base = (volatile uint32_t*)0xFEE00000;

// Read CPUID to check for APIC
static bool cpuid_has_apic(void) {
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

// Read APIC register
static uint32_t apic_read(uint32_t offset) {
    return apic_base[offset / 4];
}

// Write APIC register
static void apic_write(uint32_t offset, uint32_t value) {
    apic_base[offset / 4] = value;
}

// Check if APIC is available
bool apic_is_available(void) {
    return cpuid_has_apic();
}

// Initialize Local APIC
void apic_init(void) {
    // Enable APIC via spurious interrupt vector register
    // Set spurious vector to 0xFF and enable APIC (bit 8)
    apic_write(APIC_REG_SPURIOUS, 0x1FF);
    
    // Set Task Priority Register to 0 (accept all interrupts)
    apic_write(APIC_REG_TPR, 0);
}

// Get APIC ID
uint8_t apic_get_id(void) {
    uint32_t id_reg = apic_read(APIC_REG_ID);
    return (id_reg >> 24) & 0xFF;
}

// Send End of Interrupt
void apic_send_eoi(void) {
    apic_write(APIC_REG_EOI, 0);
}

// Send Inter-Processor Interrupt (IPI)
void apic_send_ipi(uint8_t dest_apic_id, uint8_t vector, uint32_t delivery_mode) {
    // Wait for previous IPI to complete
    while (apic_read(APIC_REG_ICR_LOW) & (1 << 12)) {
        __asm__ volatile("pause");
    }
    
    // Set destination in high register
    apic_write(APIC_REG_ICR_HIGH, ((uint32_t)dest_apic_id) << 24);
    
    // Send IPI with delivery mode and vector in low register
    apic_write(APIC_REG_ICR_LOW, delivery_mode | vector);
}
