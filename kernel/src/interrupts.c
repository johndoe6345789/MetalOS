/*
 * MetalOS Kernel - Interrupt Handling
 * 
 * Minimal IDT and interrupt handlers
 * Only essential interrupts for QT6 app
 */

#include "kernel/interrupts.h"

// IDT entries (256 interrupts in x86_64)
static idt_entry_t idt[256];
static idt_ptr_t idt_ptr;

// External ISR handlers (defined in interrupts_asm.asm)
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

// IRQ handlers
extern void irq0(void);
extern void irq1(void);

// Set an IDT entry
static void idt_set_gate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_mid = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].selector = selector;
    idt[num].ist = 0;
    idt[num].type_attr = flags;
    idt[num].zero = 0;
}

// Remap PIC (Programmable Interrupt Controller)
static void pic_remap(void) {
    // ICW1: Initialize PIC
    __asm__ volatile("outb %0, $0x20" : : "a"((uint8_t)0x11));  // Master PIC
    __asm__ volatile("outb %0, $0xA0" : : "a"((uint8_t)0x11));  // Slave PIC
    
    // ICW2: Set interrupt vector offsets
    __asm__ volatile("outb %0, $0x21" : : "a"((uint8_t)0x20));  // Master offset to 0x20
    __asm__ volatile("outb %0, $0xA1" : : "a"((uint8_t)0x28));  // Slave offset to 0x28
    
    // ICW3: Set up cascade
    __asm__ volatile("outb %0, $0x21" : : "a"((uint8_t)0x04));  // Tell master about slave
    __asm__ volatile("outb %0, $0xA1" : : "a"((uint8_t)0x02));  // Tell slave its cascade
    
    // ICW4: Set mode
    __asm__ volatile("outb %0, $0x21" : : "a"((uint8_t)0x01));
    __asm__ volatile("outb %0, $0xA1" : : "a"((uint8_t)0x01));
    
    // Mask all interrupts initially
    __asm__ volatile("outb %0, $0x21" : : "a"((uint8_t)0xFF));
    __asm__ volatile("outb %0, $0xA1" : : "a"((uint8_t)0xFF));
}

// Initialize IDT
void idt_init(void) {
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_ptr.base = (uint64_t)&idt;
    
    // Clear IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Install exception handlers (ISRs 0-31)
    idt_set_gate(0, (uint64_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint64_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint64_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint64_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint64_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint64_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint64_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint64_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint64_t)isr8, 0x08, 0x8E);
    idt_set_gate(9, (uint64_t)isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint64_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint64_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint64_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint64_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint64_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint64_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint64_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint64_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint64_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint64_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint64_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint64_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint64_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint64_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint64_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint64_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint64_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint64_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint64_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint64_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint64_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint64_t)isr31, 0x08, 0x8E);
    
    // Remap PIC
    pic_remap();
    
    // Install IRQ handlers (IRQs 0-15 mapped to 32-47)
    idt_set_gate(32, (uint64_t)irq0, 0x08, 0x8E);  // Timer
    idt_set_gate(33, (uint64_t)irq1, 0x08, 0x8E);  // Keyboard
    
    // Load IDT
    __asm__ volatile("lidt %0" : : "m"(idt_ptr));
    
    // Enable interrupts
    __asm__ volatile("sti");
}

// Generic interrupt handler
void interrupt_handler(registers_t* regs) {
    // Handle interrupt based on interrupt number
    (void)regs;  // Suppress unused warning for now
    
    // TODO: Dispatch to specific handlers based on regs->int_no
    
    // Send EOI (End of Interrupt) to PIC if this was an IRQ
    if (regs->int_no >= 32 && regs->int_no < 48) {
        if (regs->int_no >= 40) {
            // Slave PIC
            __asm__ volatile("outb %0, $0xA0" : : "a"((uint8_t)0x20));
        }
        // Master PIC
        __asm__ volatile("outb %0, $0x20" : : "a"((uint8_t)0x20));
    }
}
