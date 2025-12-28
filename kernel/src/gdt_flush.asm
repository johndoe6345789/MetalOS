; GDT flush assembly routine
; Load new GDT and update segment registers

global gdt_flush
extern gdt_ptr

section .text
bits 64

gdt_flush:
    lgdt [rdi]          ; Load GDT pointer (first argument in rdi)
    
    ; Reload segments
    mov ax, 0x10        ; Kernel data segment offset
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Far return to reload CS
    pop rdi             ; Get return address
    mov rax, 0x08       ; Kernel code segment offset
    push rax
    push rdi
    retfq               ; Far return
