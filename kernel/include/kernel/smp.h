#ifndef METALOS_KERNEL_SMP_H
#define METALOS_KERNEL_SMP_H

#include <stdint.h>
#include <stdbool.h>

// Maximum number of CPUs we support
#define MAX_CPUS 16

// Per-CPU data structure
typedef struct {
    uint8_t cpu_id;
    uint8_t apic_id;
    bool online;
    uint64_t kernel_stack;
} cpu_info_t;

#ifdef __cplusplus
// C++ SMPManager class
class SMPManager {
private:
    cpu_info_t cpuInfo[MAX_CPUS];
    uint8_t cpuCount;
    bool smpEnabled;
    
    void initCPU(uint8_t cpuId, uint8_t apicId);
    void delay(uint32_t microseconds);
    bool startAP(uint8_t apicId);

public:
    SMPManager();
    
    void init();
    uint8_t getCPUCount() const;
    uint8_t getCurrentCPU() const;
    bool isEnabled() const;
    cpu_info_t* getCPUInfo(uint8_t cpuId);
    void markCPUOnline(uint8_t cpuId);
};

extern "C" {
#endif

// C-compatible SMP functions
void smp_init(void);
uint8_t smp_get_cpu_count(void);
uint8_t smp_get_current_cpu(void);
bool smp_is_enabled(void);
cpu_info_t* smp_get_cpu_info(uint8_t cpu_id);
void smp_cpu_online(uint8_t cpu_id);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_SMP_H
