#ifndef METALOS_KERNEL_GDT_H
#define METALOS_KERNEL_GDT_H

#include <stdint.h>

// GDT Entry structure
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

// GDT Pointer structure
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_ptr_t;

// Initialize the Global Descriptor Table
void gdt_init(void);

#endif // METALOS_KERNEL_GDT_H
