/*
 * MetalOS Kernel - SMP (Symmetric Multi-Processing) Support
 * 
 * Basic multicore support for better performance
 * Initializes Application Processors (APs) using SIPI protocol
 */

#include "kernel/smp.h"
#include "kernel/apic.h"
#include "kernel/memory.h"

// Bootstrap CPU is always CPU 0
#define BSP_CPU_ID 0

// Trampoline code location (must be in low memory for real mode)
#define AP_TRAMPOLINE_ADDR 0x8000

// AP startup code (will be copied to low memory)
extern "C" {
    void ap_trampoline_start(void);
    void ap_trampoline_end(void);
}

// SMPManager class implementation
SMPManager::SMPManager() : cpuCount(1), smpEnabled(false) {
    // Initialize BSP
    cpuInfo[0].cpu_id = BSP_CPU_ID;
    cpuInfo[0].apic_id = 0;
    cpuInfo[0].online = false;
    cpuInfo[0].kernel_stack = 0;
}

uint8_t SMPManager::getCurrentCPU() const {
    if (!smpEnabled) {
        return BSP_CPU_ID;
    }
    
    uint8_t apicId = apic_get_id();
    
    // Find CPU by APIC ID
    for (uint8_t i = 0; i < cpuCount; i++) {
        if (cpuInfo[i].apic_id == apicId) {
            return cpuInfo[i].cpu_id;
        }
    }
    
    return BSP_CPU_ID;
}

void SMPManager::initCPU(uint8_t cpuId, uint8_t apicId) {
    if (cpuId >= MAX_CPUS) return;
    
    cpuInfo[cpuId].cpu_id = cpuId;
    cpuInfo[cpuId].apic_id = apicId;
    cpuInfo[cpuId].online = false;
    cpuInfo[cpuId].kernel_stack = 0;
}

void SMPManager::markCPUOnline(uint8_t cpuId) {
    if (cpuId < MAX_CPUS) {
        cpuInfo[cpuId].online = true;
    }
}

void SMPManager::delay(uint32_t microseconds) {
    // Approximate delay (not precise)
    for (volatile uint32_t i = 0; i < microseconds * 100; i++) {
        __asm__ volatile("pause");
    }
}

bool SMPManager::startAP(uint8_t apicId) {
    // Send INIT IPI
    apic_send_ipi(apicId, 0, APIC_IPI_INIT);
    delay(10000);  // 10ms
    
    // Send SIPI (Startup IPI) with trampoline address
    uint8_t vector = AP_TRAMPOLINE_ADDR >> 12;  // Page number
    apic_send_ipi(apicId, vector, APIC_IPI_STARTUP);
    delay(200);  // 200us
    
    // Send second SIPI (as per Intel spec)
    apic_send_ipi(apicId, vector, APIC_IPI_STARTUP);
    delay(200);  // 200us
    
    // Wait for AP to come online (timeout after 1 second)
    for (int i = 0; i < 100; i++) {
        // Check if AP marked itself online
        for (uint8_t cpuId = 0; cpuId < cpuCount; cpuId++) {
            if (cpuInfo[cpuId].apic_id == apicId && cpuInfo[cpuId].online) {
                return true;
            }
        }
        delay(10000);  // 10ms
    }
    
    return false;
}

void SMPManager::init() {
    // Check if APIC is available
    if (!apic_is_available()) {
        // Single core mode
        initCPU(BSP_CPU_ID, 0);
        cpuInfo[BSP_CPU_ID].online = true;
        cpuCount = 1;
        smpEnabled = false;
        return;
    }
    
    // Initialize APIC
    apic_init();
    
    // Get BSP APIC ID
    uint8_t bspApicId = apic_get_id();
    initCPU(BSP_CPU_ID, bspApicId);
    cpuInfo[BSP_CPU_ID].online = true;
    
    // Detect additional CPUs from APIC
    uint8_t maxCPUsToTry = 12;  // Try up to 12 logical processors
    
    for (uint8_t apicId = 0; apicId < maxCPUsToTry && cpuCount < MAX_CPUS; apicId++) {
        // Skip BSP
        if (apicId == bspApicId) {
            continue;
        }
        
        // Initialize CPU entry
        initCPU(cpuCount, apicId);
        
        // Try to start this AP
        if (startAP(apicId)) {
            cpuCount++;
        }
    }
    
    smpEnabled = (cpuCount > 1);
}

uint8_t SMPManager::getCPUCount() const {
    return cpuCount;
}

bool SMPManager::isEnabled() const {
    return smpEnabled;
}

cpu_info_t* SMPManager::getCPUInfo(uint8_t cpuId) {
    if (cpuId >= MAX_CPUS) {
        return nullptr;
    }
    return &cpuInfo[cpuId];
}

// Global SMP manager instance
static SMPManager globalSMP;

// C-compatible wrapper functions
extern "C" {

void smp_init(void) {
    globalSMP.init();
}

uint8_t smp_get_cpu_count(void) {
    return globalSMP.getCPUCount();
}

uint8_t smp_get_current_cpu(void) {
    return globalSMP.getCurrentCPU();
}

bool smp_is_enabled(void) {
    return globalSMP.isEnabled();
}

cpu_info_t* smp_get_cpu_info(uint8_t cpu_id) {
    return globalSMP.getCPUInfo(cpu_id);
}

void smp_cpu_online(uint8_t cpu_id) {
    globalSMP.markCPUOnline(cpu_id);
}

} // extern "C"
