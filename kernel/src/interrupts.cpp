/**
 * @file interrupts.cpp
 * @brief Implementation of interrupt descriptor table and interrupt handling
 * 
 * Manages CPU exceptions and hardware interrupts through the IDT.
 * Supports both legacy PIC and modern APIC interrupt controllers.
 */

#include "kernel/interrupts.h"
#include "kernel/timer.h"
#include "kernel/smp.h"
#include "kernel/apic.h"

/**
 * @brief Write a byte to an I/O port
 * @param port I/O port address
 * @param value Byte value to write
 */
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

/* InterruptManager class implementation */

/**
 * @brief Constructor - initializes IDT pointer structure
 */
InterruptManager::InterruptManager() {
    idtPtr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idtPtr.base = (uint64_t)&idt;
}

/**
 * @brief Set an IDT entry to point to an interrupt handler
 * 
 * In 64-bit mode, IDT entries are 16 bytes and contain:
 * - 64-bit handler address (split across three fields)
 * - 16-bit code segment selector
 * - Type and attributes (present, DPL, gate type)
 * - IST (Interrupt Stack Table) offset (usually 0)
 * 
 * @param num Interrupt vector number (0-255)
 * @param handler Address of interrupt handler function
 * @param selector Code segment selector (0x08 for kernel code)
 * @param flags Type and attribute byte:
 *              - Bit 7: Present (1)
 *              - Bits 5-6: DPL (0 for kernel)
 *              - Bits 0-4: Gate type (0xE for interrupt gate)
 *              Common value: 0x8E = Present, DPL=0, Interrupt Gate
 */
void InterruptManager::setGate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_mid = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].selector = selector;
    idt[num].ist = 0;
    idt[num].type_attr = flags;
    idt[num].zero = 0;
}

/**
 * @brief Remap the 8259 PIC to avoid conflicts with CPU exceptions
 * 
 * By default, the PIC uses IRQ vectors 0-15, which overlap with CPU exception
 * vectors 0-31. This causes confusion when a hardware interrupt has the same
 * vector as a CPU exception (e.g., IRQ 8 vs Double Fault exception 8).
 * 
 * We remap the PIC so that:
 * - Master PIC (IRQ 0-7) → vectors 32-39
 * - Slave PIC (IRQ 8-15) → vectors 40-47
 * 
 * The remapping process uses ICW (Initialization Command Words):
 * - ICW1: Start initialization (0x11 = ICW4 needed, cascade mode)
 * - ICW2: Set vector offset (0x20 for master, 0x28 for slave)
 * - ICW3: Set up cascade (master: slave on IRQ2, slave: cascade identity)
 * - ICW4: Set 8086 mode
 * 
 * After remapping, all IRQs are masked (disabled) initially. Individual IRQs
 * must be explicitly unmasked to receive interrupts.
 */
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

/**
 * @brief Initialize the IDT and enable interrupts
 * 
 * This function performs complete interrupt subsystem initialization:
 * 1. Clear all 256 IDT entries
 * 2. Install exception handlers (ISR 0-31) for CPU exceptions
 * 3. Remap the PIC to avoid conflicts
 * 4. Install IRQ handlers (32-47) for hardware interrupts
 * 5. Load IDT using LIDT instruction
 * 6. Enable interrupts using STI instruction
 * 
 * CPU exceptions (0-31) include:
 * - 0: Divide by zero
 * - 6: Invalid opcode
 * - 13: General protection fault
 * - 14: Page fault
 * etc.
 * 
 * Hardware IRQs (32-47) include:
 * - 32 (IRQ 0): Timer
 * - 33 (IRQ 1): Keyboard
 * - 44 (IRQ 12): PS/2 Mouse
 * etc.
 */
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

/**
 * @brief Main interrupt handler dispatcher
 * 
 * This function is called from the assembly interrupt stubs (ISRs/IRQs).
 * It receives the saved CPU state and dispatches to specific handlers
 * based on the interrupt number.
 * 
 * Process:
 * 1. Check interrupt number
 * 2. Call specific handler if needed (e.g., timer for IRQ 0)
 * 3. Send End-Of-Interrupt signal to PIC or APIC
 * 
 * For hardware IRQs (32-47):
 * - Check if using APIC (multicore) or PIC (legacy)
 * - Send EOI to appropriate controller
 * - For slave PIC IRQs (40-47), must send EOI to both PICs
 * 
 * @param regs Pointer to saved CPU register state
 */
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
