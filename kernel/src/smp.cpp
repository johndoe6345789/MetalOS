/**
 * @file smp.cpp
 * @brief Implementation of SMP (Symmetric Multi-Processing) initialization
 * 
 * SMP support allows the OS to use multiple CPU cores. This involves starting
 * Application Processors (APs) using the INIT-SIPI-SIPI sequence defined by Intel.
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

/* SMPManager class implementation */

/**
 * @brief Constructor - initializes BSP (Bootstrap Processor) as CPU 0
 * 
 * The BSP is the first CPU core that starts when the system boots.
 * It's responsible for initializing the system and starting other cores (APs).
 */
SMPManager::SMPManager() : cpuCount(1), smpEnabled(false) {
    // Initialize BSP
    cpuInfo[0].cpu_id = BSP_CPU_ID;
    cpuInfo[0].apic_id = 0;
    cpuInfo[0].online = false;
    cpuInfo[0].kernel_stack = 0;
}

/**
 * @brief Get the logical CPU ID of the currently executing core
 * 
 * Uses the Local APIC ID to determine which CPU is running this code.
 * This is important in multicore systems where each core may be executing
 * kernel code simultaneously.
 * 
 * @return Logical CPU ID (0 for BSP, 1+ for APs)
 */
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

/**
 * @brief Initialize CPU information structure
 * 
 * Sets up the per-CPU data structure with initial values.
 * Each CPU has a logical ID (sequential: 0, 1, 2...) and a physical
 * APIC ID (may not be sequential, e.g., 0, 2, 4, 6...).
 * 
 * @param cpuId Logical CPU ID (0-15)
 * @param apicId Physical APIC ID
 */
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

/**
 * @brief Busy-wait delay for timing during AP startup
 * 
 * This is an approximate delay using a busy loop. Not precise, but sufficient
 * for the timing requirements of the INIT-SIPI-SIPI sequence:
 * - 10ms delay after INIT
 * - 200μs delay after each SIPI
 * 
 * @param microseconds Delay duration in microseconds (approximate)
 * @note Uses PAUSE instruction to improve performance during busy-wait
 */
void SMPManager::delay(uint32_t microseconds) {
    // Approximate delay (not precise)
    for (volatile uint32_t i = 0; i < microseconds * 100; i++) {
        __asm__ volatile("pause");
    }
}

/**
 * @brief Start an Application Processor using INIT-SIPI-SIPI sequence
 * 
 * The Intel-specified AP startup sequence:
 * 1. Send INIT IPI to reset the AP to a known state (16-bit real mode)
 * 2. Wait 10ms for INIT to complete
 * 3. Send first SIPI with vector = page number of trampoline code
 * 4. Wait 200μs
 * 5. Send second SIPI (per Intel spec for compatibility)
 * 6. Wait 200μs
 * 7. Poll for AP to mark itself online (timeout after 1 second)
 * 
 * The SIPI vector is the page number (4KB) where the trampoline code
 * is located. For address 0x8000, vector = 0x8000 >> 12 = 0x08.
 * 
 * The trampoline code must:
 * - Be in low memory (< 1MB) accessible in real mode
 * - Switch from 16-bit real mode to 64-bit long mode
 * - Initialize the AP's GDT, IDT, and APIC
 * - Jump to the AP entry point in the kernel
 * 
 * @param apicId Physical APIC ID of the AP to start
 * @return true if AP started successfully, false on timeout
 */
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

/**
 * @brief Initialize SMP subsystem and start all available CPU cores
 * 
 * This function performs the following steps:
 * 1. Check if Local APIC is available (required for SMP)
 * 2. If no APIC, fall back to single-core mode
 * 3. Initialize BSP's Local APIC
 * 4. Get BSP's APIC ID
 * 5. Attempt to start additional cores by sending IPIs
 * 6. Set smpEnabled flag if multiple cores detected
 * 
 * The function tries to start up to MAX_CPUS cores by probing APIC IDs
 * from 0 to maxCPUsToTry. In a real system, this should be done by
 * parsing the ACPI MADT (Multiple APIC Description Table) to find
 * the actual APIC IDs of installed CPUs.
 * 
 * After successful initialization:
 * - All cores have initialized their Local APICs
 * - All cores are marked as online
 * - Each core can execute kernel code
 * - Currently only BSP runs the application (APs idle)
 */
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
