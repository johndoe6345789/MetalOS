; AP (Application Processor) Trampoline Code
; This code runs in real mode and brings up secondary CPUs
; Must be located in low memory (< 1MB) for real mode addressing

bits 16
section .text

global ap_trampoline_start
global ap_trampoline_end

ap_trampoline_start:
    cli                         ; Disable interrupts
    
    ; Load GDT
    lgdt [ap_gdt_desc - ap_trampoline_start + 0x8000]
    
    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to 32-bit code
    jmp 0x08:(ap_protected_mode - ap_trampoline_start + 0x8000)

bits 32
ap_protected_mode:
    ; Set up segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Enable PAE and PSE
    mov eax, cr4
    or eax, 0x20 | 0x10         ; CR4.PAE | CR4.PSE
    mov cr4, eax
    
    ; Load PML4 (page table) - for now, use identity mapping
    ; In a real implementation, this should be passed from BSP
    mov eax, 0x1000             ; Placeholder
    mov cr3, eax
    
    ; Enable long mode
    mov ecx, 0xC0000080         ; EFER MSR
    rdmsr
    or eax, 0x100               ; EFER.LME
    wrmsr
    
    ; Enable paging
    mov eax, cr0
    or eax, 0x80000000          ; CR0.PG
    mov cr0, eax
    
    ; Jump to 64-bit code
    jmp 0x08:ap_long_mode

bits 64
ap_long_mode:
    ; AP is now in 64-bit mode
    ; Mark CPU as online and halt
    ; (In real impl, would jump to AP entry point)
    
    ; Get APIC ID and mark online
    mov rax, 1
    cpuid
    shr rbx, 24                 ; APIC ID in high byte
    
    ; For now, just halt - BSP will detect we came online
    cli
.halt:
    hlt
    jmp .halt

; GDT for AP startup
align 8
ap_gdt:
    dq 0x0000000000000000       ; Null descriptor
    dq 0x00CF9A000000FFFF       ; Code segment (32-bit)
    dq 0x00CF92000000FFFF       ; Data segment
    dq 0x00AF9A000000FFFF       ; Code segment (64-bit)
ap_gdt_end:

ap_gdt_desc:
    dw ap_gdt_end - ap_gdt - 1  ; Limit
    dd ap_gdt - ap_trampoline_start + 0x8000  ; Base

ap_trampoline_end:
