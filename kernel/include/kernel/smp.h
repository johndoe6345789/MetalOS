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

// SMP initialization
void smp_init(void);

// Get number of CPUs detected
uint8_t smp_get_cpu_count(void);

// Get current CPU ID
uint8_t smp_get_current_cpu(void);

// Check if SMP is enabled
bool smp_is_enabled(void);

// Get CPU info
cpu_info_t* smp_get_cpu_info(uint8_t cpu_id);

// Mark CPU as online (internal use by AP startup)
void smp_cpu_online(uint8_t cpu_id);

#endif // METALOS_KERNEL_SMP_H
