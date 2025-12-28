#ifndef METALOS_KERNEL_PCI_H
#define METALOS_KERNEL_PCI_H

#include <stdint.h>

// PCI Configuration Space Registers
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// Maximum devices we'll track
#define MAX_PCI_DEVICES 256

// PCI Device Structure
typedef struct {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t revision_id;
    uint32_t bar[6];  // Base Address Registers
} pci_device_t;

#ifdef __cplusplus
// C++ PCIDevice class-compatible structure
struct PCIDevice {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t revision_id;
    uint32_t bar[6];
};

// C++ PCIManager class
class PCIManager {
private:
    PCIDevice devices[MAX_PCI_DEVICES];
    uint32_t deviceCount;
    
    void probeDevice(uint8_t bus, uint8_t device, uint8_t function);

public:
    PCIManager();
    
    void init();
    uint32_t readConfig(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
    void writeConfig(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
    PCIDevice* findDevice(uint16_t vendor_id, uint16_t device_id);
    void enableBusMastering(PCIDevice* dev);
};

extern "C" {
#endif

// C-compatible PCI functions
void pci_init(void);
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id);
void pci_enable_bus_mastering(pci_device_t* dev);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_PCI_H
