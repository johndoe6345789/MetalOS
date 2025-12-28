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

// APIC functions
bool apic_is_available(void);
void apic_init(void);
uint8_t apic_get_id(void);
void apic_send_eoi(void);
void apic_send_ipi(uint8_t dest_apic_id, uint8_t vector, uint32_t delivery_mode);

#endif // METALOS_KERNEL_APIC_H
