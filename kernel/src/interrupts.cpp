/*
 * MetalOS Kernel - Interrupt Handling
 * 
 * Minimal IDT and interrupt handlers
 * Supports both PIC (legacy) and APIC (multicore) modes
 */

#include "kernel/interrupts.h"
#include "kernel/timer.h"
#include "kernel/smp.h"
#include "kernel/apic.h"

// I/O port access functions
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// External ISR handlers (defined in interrupts_asm.asm)
extern "C" {
    void isr0(void); void isr1(void); void isr2(void); void isr3(void);
    void isr4(void); void isr5(void); void isr6(void); void isr7(void);
    void isr8(void); void isr9(void); void isr10(void); void isr11(void);
    void isr12(void); void isr13(void); void isr14(void); void isr15(void);
    void isr16(void); void isr17(void); void isr18(void); void isr19(void);
    void isr20(void); void isr21(void); void isr22(void); void isr23(void);
    void isr24(void); void isr25(void); void isr26(void); void isr27(void);
    void isr28(void); void isr29(void); void isr30(void); void isr31(void);
    void irq0(void); void irq1(void);
}

// InterruptManager class implementation
InterruptManager::InterruptManager() {
    idtPtr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idtPtr.base = (uint64_t)&idt;
}

void InterruptManager::setGate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_mid = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].selector = selector;
    idt[num].ist = 0;
    idt[num].type_attr = flags;
    idt[num].zero = 0;
}

void InterruptManager::remapPIC() {
    // ICW1: Initialize PIC
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    
    // ICW2: Set interrupt vector offsets
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    
    // ICW3: Set up cascade
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    
    // ICW4: Set mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // Mask all interrupts initially
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void InterruptManager::init() {
    // Clear IDT
    for (int i = 0; i < 256; i++) {
        setGate(i, 0, 0, 0);
    }
    
    // Install exception handlers (ISRs 0-31)
    setGate(0, (uint64_t)isr0, 0x08, 0x8E);
    setGate(1, (uint64_t)isr1, 0x08, 0x8E);
    setGate(2, (uint64_t)isr2, 0x08, 0x8E);
    setGate(3, (uint64_t)isr3, 0x08, 0x8E);
    setGate(4, (uint64_t)isr4, 0x08, 0x8E);
    setGate(5, (uint64_t)isr5, 0x08, 0x8E);
    setGate(6, (uint64_t)isr6, 0x08, 0x8E);
    setGate(7, (uint64_t)isr7, 0x08, 0x8E);
    setGate(8, (uint64_t)isr8, 0x08, 0x8E);
    setGate(9, (uint64_t)isr9, 0x08, 0x8E);
    setGate(10, (uint64_t)isr10, 0x08, 0x8E);
    setGate(11, (uint64_t)isr11, 0x08, 0x8E);
    setGate(12, (uint64_t)isr12, 0x08, 0x8E);
    setGate(13, (uint64_t)isr13, 0x08, 0x8E);
    setGate(14, (uint64_t)isr14, 0x08, 0x8E);
    setGate(15, (uint64_t)isr15, 0x08, 0x8E);
    setGate(16, (uint64_t)isr16, 0x08, 0x8E);
    setGate(17, (uint64_t)isr17, 0x08, 0x8E);
    setGate(18, (uint64_t)isr18, 0x08, 0x8E);
    setGate(19, (uint64_t)isr19, 0x08, 0x8E);
    setGate(20, (uint64_t)isr20, 0x08, 0x8E);
    setGate(21, (uint64_t)isr21, 0x08, 0x8E);
    setGate(22, (uint64_t)isr22, 0x08, 0x8E);
    setGate(23, (uint64_t)isr23, 0x08, 0x8E);
    setGate(24, (uint64_t)isr24, 0x08, 0x8E);
    setGate(25, (uint64_t)isr25, 0x08, 0x8E);
    setGate(26, (uint64_t)isr26, 0x08, 0x8E);
    setGate(27, (uint64_t)isr27, 0x08, 0x8E);
    setGate(28, (uint64_t)isr28, 0x08, 0x8E);
    setGate(29, (uint64_t)isr29, 0x08, 0x8E);
    setGate(30, (uint64_t)isr30, 0x08, 0x8E);
    setGate(31, (uint64_t)isr31, 0x08, 0x8E);
    
    // Remap PIC
    remapPIC();
    
    // Install IRQ handlers (IRQs 0-15 mapped to 32-47)
    setGate(32, (uint64_t)irq0, 0x08, 0x8E);
    setGate(33, (uint64_t)irq1, 0x08, 0x8E);
    
    // Load IDT
    __asm__ volatile("lidt %0" : : "m"(idtPtr));
    
    // Enable interrupts
    __asm__ volatile("sti");
}

void InterruptManager::handleInterrupt(registers_t* regs) {
    // Handle specific interrupts
    if (regs->int_no == 32) {
        // Timer interrupt (IRQ0)
        timer_handler();
    }
    
    // Send EOI (End of Interrupt)
    if (regs->int_no >= 32 && regs->int_no < 48) {
        // Check if we're using APIC or PIC
        if (smp_is_enabled() && apic_is_available()) {
            // Use APIC EOI
            apic_send_eoi();
        } else {
            // Use legacy PIC EOI
            if (regs->int_no >= 40) {
                outb(PIC2_COMMAND, 0x20);
            }
            outb(PIC1_COMMAND, 0x20);
        }
    }
}

// Global interrupt manager instance
static InterruptManager globalInterruptManager;

// C-compatible wrapper functions
extern "C" {

void idt_init(void) {
    globalInterruptManager.init();
}

void interrupt_handler(registers_t* regs) {
    globalInterruptManager.handleInterrupt(regs);
}

} // extern "C"
