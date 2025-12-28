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

#ifdef __cplusplus
// C++ GDT class
class GDT {
private:
    gdt_entry_t entries[5];
    gdt_ptr_t gdtPtr;
    
    void setGate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

public:
    GDT();
    
    void init();
};

extern "C" {
#endif

// C-compatible GDT function
void gdt_init(void);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_GDT_H
