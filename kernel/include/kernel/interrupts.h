#ifndef METALOS_KERNEL_INTERRUPTS_H
#define METALOS_KERNEL_INTERRUPTS_H

#include <stdint.h>

/**
 * @struct idt_entry_t
 * @brief Interrupt Descriptor Table entry structure (16 bytes in 64-bit mode)
 * 
 * Each IDT entry describes how to handle a specific interrupt or exception.
 * The IDT contains 256 entries for interrupt vectors 0-255:
 * - Vectors 0-31: CPU exceptions (divide by zero, page fault, etc.)
 * - Vectors 32-47: Hardware IRQs (after PIC remap)
 * - Vectors 48-255: Available for software interrupts
 * 
 * In 64-bit mode, IDT entries are 16 bytes and point to interrupt handler functions.
 */
typedef struct {
    uint16_t offset_low;    ///< Lower 16 bits of handler function address
    uint16_t selector;      ///< Code segment selector (typically 0x08 for kernel code)
    uint8_t ist;            ///< Interrupt Stack Table offset (0 = use current stack)
    uint8_t type_attr;      ///< Type and attributes (present, DPL, gate type)
    uint16_t offset_mid;    ///< Middle 16 bits of handler function address
    uint32_t offset_high;   ///< Upper 32 bits of handler function address
    uint32_t zero;          ///< Reserved, must be zero
} __attribute__((packed)) idt_entry_t;

/**
 * @struct idt_ptr_t
 * @brief IDT pointer structure used by LIDT instruction
 * 
 * This structure is loaded into the CPU using the LIDT (Load IDT) instruction.
 * It tells the CPU where the IDT is located in memory and how large it is.
 */
typedef struct {
    uint16_t limit;  ///< Size of IDT in bytes minus 1
    uint64_t base;   ///< 64-bit linear address of the IDT
} __attribute__((packed)) idt_ptr_t;

/**
 * @struct registers_t
 * @brief CPU register state saved during interrupt handling
 * 
 * This structure represents the complete CPU state at the time an interrupt occurred.
 * It is pushed onto the stack by the interrupt handler assembly code and passed to
 * the C interrupt handler. It includes:
 * - General purpose registers (rax, rbx, rcx, etc.)
 * - Interrupt number and error code
 * - Execution state (rip, rsp, rflags) automatically pushed by CPU
 */
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;  ///< Extended GP registers
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;     ///< Standard GP registers
    uint64_t int_no, err_code;                       ///< Interrupt number and error code
    uint64_t rip, cs, rflags, rsp, ss;              ///< CPU state (pushed by CPU)
} __attribute__((packed)) registers_t;

#ifdef __cplusplus
/**
 * @class InterruptManager
 * @brief Manages the Interrupt Descriptor Table and interrupt handling
 * 
 * The InterruptManager class is responsible for:
 * - Setting up the IDT with 256 interrupt vectors
 * - Installing interrupt handler functions
 * - Remapping the legacy 8259 PIC (Programmable Interrupt Controller)
 * - Dispatching interrupts to appropriate handlers
 * - Sending End-Of-Interrupt signals to PIC or APIC
 * 
 * Key concepts:
 * - ISR (Interrupt Service Routine): Handles CPU exceptions (0-31)
 * - IRQ (Interrupt Request): Handles hardware interrupts (32-47 after remap)
 * - PIC (Programmable Interrupt Controller): Legacy 8259 chip that manages IRQs
 * - APIC (Advanced PIC): Modern interrupt controller for multicore systems
 * 
 * The PIC is remapped because the default IRQ vectors (0-15) conflict with
 * CPU exception vectors. We remap IRQs to vectors 32-47.
 */
class InterruptManager {
private:
    idt_entry_t idt[256];  ///< Array of 256 IDT entries
    idt_ptr_t idtPtr;      ///< IDT pointer for LIDT instruction
    
    /**
     * @brief Set an IDT entry to point to an interrupt handler
     * @param num Interrupt vector number (0-255)
     * @param handler Address of interrupt handler function
     * @param selector Code segment selector (0x08 for kernel code)
     * @param flags Type and attribute flags (present, DPL, gate type)
     */
    void setGate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t flags);
    
    /**
     * @brief Remap the 8259 PIC to use different IRQ vectors
     * @note Remaps IRQ 0-7 to vectors 32-39, IRQ 8-15 to vectors 40-47
     * @note Masks all IRQs initially; they must be explicitly unmasked to receive
     */
    void remapPIC();

public:
    /** @brief Constructor - initializes IDT pointer structure */
    InterruptManager();
    
    /**
     * @brief Initialize the IDT and enable interrupts
     * @note Sets up all 256 IDT entries
     * @note Installs exception handlers (ISR 0-31)
     * @note Remaps and configures the PIC
     * @note Installs IRQ handlers (IRQ 0-15 → vectors 32-47)
     * @note Loads IDT using LIDT instruction
     * @note Enables interrupts using STI instruction
     */
    void init();
    
    /**
     * @brief Handle an interrupt by dispatching to appropriate handler
     * @param regs Pointer to saved CPU register state
     * @note Calls specific handlers based on interrupt number
     * @note Sends EOI to PIC or APIC after handling
     */
    void handleInterrupt(registers_t* regs);
};

extern "C" {
#endif

/* Interrupt Management - C-compatible interface */

/**
 * @brief Initialize the Interrupt Descriptor Table and enable interrupts
 * @note Must be called after GDT initialization
 */
void idt_init(void);

/**
 * @brief Main interrupt handler dispatcher (called from assembly ISRs)
 * @param regs Pointer to saved CPU register state
 * @note Should only be called from interrupt context
 */
void interrupt_handler(registers_t* regs);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_INTERRUPTS_H
