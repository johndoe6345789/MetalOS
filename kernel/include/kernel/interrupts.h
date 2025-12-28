#ifndef METALOS_KERNEL_INTERRUPTS_H
#define METALOS_KERNEL_INTERRUPTS_H

#include <stdint.h>

// IDT Entry structure
typedef struct {
    uint16_t offset_low;    // Offset bits 0-15
    uint16_t selector;      // Code segment selector
    uint8_t ist;            // Interrupt Stack Table offset
    uint8_t type_attr;      // Type and attributes
    uint16_t offset_mid;    // Offset bits 16-31
    uint32_t offset_high;   // Offset bits 32-63
    uint32_t zero;          // Reserved
} __attribute__((packed)) idt_entry_t;

// IDT Pointer structure
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_ptr_t;

// CPU registers state (for interrupt handlers)
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) registers_t;

// Initialize Interrupt Descriptor Table
void idt_init(void);

// Generic interrupt handler (called from assembly)
void interrupt_handler(registers_t* regs);

#endif // METALOS_KERNEL_INTERRUPTS_H
