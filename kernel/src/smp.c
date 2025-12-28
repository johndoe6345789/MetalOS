/*
 * MetalOS Kernel - SMP (Symmetric Multi-Processing) Support
 * 
 * Basic multicore support for better performance
 * Initializes Application Processors (APs) using SIPI protocol
 */

#include "kernel/smp.h"
#include "kernel/apic.h"
#include "kernel/memory.h"

// CPU information array
static cpu_info_t cpu_info[MAX_CPUS];
static uint8_t cpu_count = 1;  // Start with BSP
static bool smp_enabled = false;

// Bootstrap CPU is always CPU 0
#define BSP_CPU_ID 0

// Trampoline code location (must be in low memory for real mode)
#define AP_TRAMPOLINE_ADDR 0x8000

// AP startup code (will be copied to low memory)
extern void ap_trampoline_start(void);
extern void ap_trampoline_end(void);

// Get current CPU ID from APIC
uint8_t smp_get_current_cpu(void) {
    if (!smp_enabled) {
        return BSP_CPU_ID;
    }
    
    uint8_t apic_id = apic_get_id();
    
    // Find CPU by APIC ID
    for (uint8_t i = 0; i < cpu_count; i++) {
        if (cpu_info[i].apic_id == apic_id) {
            return cpu_info[i].cpu_id;
        }
    }
    
    return BSP_CPU_ID;
}

// Initialize a CPU entry
static void smp_init_cpu(uint8_t cpu_id, uint8_t apic_id) {
    if (cpu_id >= MAX_CPUS) return;
    
    cpu_info[cpu_id].cpu_id = cpu_id;
    cpu_info[cpu_id].apic_id = apic_id;
    cpu_info[cpu_id].online = false;
    cpu_info[cpu_id].kernel_stack = 0;
}

// Mark CPU as online
void smp_cpu_online(uint8_t cpu_id) {
    if (cpu_id < MAX_CPUS) {
        cpu_info[cpu_id].online = true;
    }
}

// Simple delay for AP startup
static void smp_delay(uint32_t microseconds) {
    // Approximate delay (not precise)
    for (volatile uint32_t i = 0; i < microseconds * 100; i++) {
        __asm__ volatile("pause");
    }
}

// Start an Application Processor
static bool smp_start_ap(uint8_t apic_id) {
    // Send INIT IPI
    apic_send_ipi(apic_id, 0, APIC_IPI_INIT);
    smp_delay(10000);  // 10ms
    
    // Send SIPI (Startup IPI) with trampoline address
    uint8_t vector = AP_TRAMPOLINE_ADDR >> 12;  // Page number
    apic_send_ipi(apic_id, vector, APIC_IPI_STARTUP);
    smp_delay(200);  // 200us
    
    // Send second SIPI (as per Intel spec)
    apic_send_ipi(apic_id, vector, APIC_IPI_STARTUP);
    smp_delay(200);  // 200us
    
    // Wait for AP to come online (timeout after 1 second)
    for (int i = 0; i < 100; i++) {
        // Check if AP marked itself online
        for (uint8_t cpu_id = 0; cpu_id < cpu_count; cpu_id++) {
            if (cpu_info[cpu_id].apic_id == apic_id && cpu_info[cpu_id].online) {
                return true;
            }
        }
        smp_delay(10000);  // 10ms
    }
    
    return false;
}

// Initialize SMP
void smp_init(void) {
    // Check if APIC is available
    if (!apic_is_available()) {
        // Single core mode
        smp_init_cpu(BSP_CPU_ID, 0);
        cpu_info[BSP_CPU_ID].online = true;
        cpu_count = 1;
        smp_enabled = false;
        return;
    }
    
    // Initialize APIC
    apic_init();
    
    // Get BSP APIC ID
    uint8_t bsp_apic_id = apic_get_id();
    smp_init_cpu(BSP_CPU_ID, bsp_apic_id);
    cpu_info[BSP_CPU_ID].online = true;
    
    // Detect additional CPUs from APIC
    // For simplicity, we'll try to start CPUs with sequential APIC IDs
    // A real implementation would parse ACPI MADT table
    
    uint8_t max_cpus_to_try = 12;  // Try up to 12 logical processors
    
    for (uint8_t apic_id = 0; apic_id < max_cpus_to_try && cpu_count < MAX_CPUS; apic_id++) {
        // Skip BSP
        if (apic_id == bsp_apic_id) {
            continue;
        }
        
        // Initialize CPU entry
        smp_init_cpu(cpu_count, apic_id);
        
        // Try to start this AP
        if (smp_start_ap(apic_id)) {
            cpu_count++;
        }
    }
    
    smp_enabled = (cpu_count > 1);
}

// Get number of CPUs
uint8_t smp_get_cpu_count(void) {
    return cpu_count;
}

// Check if SMP is enabled
bool smp_is_enabled(void) {
    return smp_enabled;
}

// Get CPU info
cpu_info_t* smp_get_cpu_info(uint8_t cpu_id) {
    if (cpu_id >= MAX_CPUS) {
        return NULL;
    }
    return &cpu_info[cpu_id];
}
