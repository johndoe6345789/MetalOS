#ifndef METALOS_KERNEL_SMP_H
#define METALOS_KERNEL_SMP_H

#include <stdint.h>
#include <stdbool.h>

// Maximum number of CPUs we support
#define MAX_CPUS 16

/**
 * @struct cpu_info_t
 * @brief Per-CPU information structure
 * 
 * Stores information about each CPU core in a multicore system.
 */
typedef struct {
    uint8_t cpu_id;       ///< Logical CPU ID (0, 1, 2, ...)
    uint8_t apic_id;      ///< Physical APIC ID (may not be sequential)
    bool online;          ///< True if CPU is initialized and running
    uint64_t kernel_stack; ///< Kernel stack pointer for this CPU (future use)
} cpu_info_t;

#ifdef __cplusplus
/**
 * @class SMPManager
 * @brief Symmetric Multi-Processing manager for multicore CPU initialization
 * 
 * SMP (Symmetric Multi-Processing) allows an operating system to use multiple
 * CPU cores. In x86-64 systems:
 * - One core (BSP - Bootstrap Processor) starts first and initializes the system
 * - Other cores (APs - Application Processors) must be explicitly started by the BSP
 * 
 * The SMP initialization process:
 * 1. BSP initializes its own Local APIC
 * 2. BSP discovers other cores (from ACPI tables or by probing)
 * 3. BSP sends INIT IPI to each AP (reset the core)
 * 4. BSP sends SIPI (Startup IPI) with entry point address to each AP
 * 5. AP starts executing at entry point, initializes itself
 * 6. AP marks itself as online
 * 
 * The SIPI protocol requires:
 * - A small "trampoline" code in low memory (below 1MB) in real mode
 * - Two SIPI messages sent with specific timing (per Intel spec)
 * - Waiting for AP to signal it's online
 * 
 * Once all cores are initialized, they can be used for parallel processing,
 * though MetalOS currently only uses the BSP for application execution.
 */
class SMPManager {
private:
    cpu_info_t cpuInfo[MAX_CPUS];  ///< Information for each CPU core
    uint8_t cpuCount;               ///< Total number of CPU cores detected
    bool smpEnabled;                ///< True if multiple cores are available
    
    /**
     * @brief Initialize CPU info structure for a core
     * @param cpuId Logical CPU ID
     * @param apicId Physical APIC ID
     */
    void initCPU(uint8_t cpuId, uint8_t apicId);
    
    /**
     * @brief Busy-wait delay (approximate)
     * @param microseconds Delay duration in microseconds
     * @note Not precise, used for timing during AP startup
     */
    void delay(uint32_t microseconds);
    
    /**
     * @brief Start an Application Processor using SIPI protocol
     * @param apicId APIC ID of the AP to start
     * @return true if AP started successfully, false on timeout
     * @note Sends INIT IPI followed by two SIPI messages
     */
    bool startAP(uint8_t apicId);

public:
    /** @brief Constructor - initializes BSP as first CPU */
    SMPManager();
    
    /**
     * @brief Initialize SMP subsystem and start all available CPU cores
     * @note Checks for APIC availability
     * @note Initializes BSP's APIC
     * @note Attempts to start up to MAX_CPUS cores
     * @note Falls back to single-core mode if APIC unavailable
     */
    void init();
    
    /**
     * @brief Get total number of CPU cores detected
     * @return Number of CPU cores (at least 1)
     */
    uint8_t getCPUCount() const;
    
    /**
     * @brief Get logical ID of the currently executing CPU core
     * @return CPU ID (0 for BSP)
     */
    uint8_t getCurrentCPU() const;
    
    /**
     * @brief Check if SMP is enabled (multiple cores available)
     * @return true if multiple cores detected, false if single-core
     */
    bool isEnabled() const;
    
    /**
     * @brief Get information structure for a specific CPU
     * @param cpuId Logical CPU ID
     * @return Pointer to cpu_info_t structure, or nullptr if invalid ID
     */
    cpu_info_t* getCPUInfo(uint8_t cpuId);
    
    /**
     * @brief Mark a CPU as online (called by AP during initialization)
     * @param cpuId Logical CPU ID
     */
    void markCPUOnline(uint8_t cpuId);
};

extern "C" {
#endif

/* SMP - C-compatible interface */

/**
 * @brief Initialize SMP subsystem and start all CPU cores
 * @note Should be called after APIC initialization
 */
void smp_init(void);

/**
 * @brief Get number of CPU cores detected
 * @return Number of CPU cores
 */
uint8_t smp_get_cpu_count(void);

/**
 * @brief Get ID of current CPU core
 * @return Logical CPU ID
 */
uint8_t smp_get_current_cpu(void);

/**
 * @brief Check if SMP is enabled
 * @return true if multiple cores available, false if single-core
 */
bool smp_is_enabled(void);

/**
 * @brief Get CPU information structure
 * @param cpu_id Logical CPU ID
 * @return Pointer to cpu_info_t structure or NULL
 */
cpu_info_t* smp_get_cpu_info(uint8_t cpu_id);

/**
 * @brief Mark CPU as online (called by AP during startup)
 * @param cpu_id Logical CPU ID
 */
void smp_cpu_online(uint8_t cpu_id);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_SMP_H
