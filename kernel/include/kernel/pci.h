#ifndef METALOS_KERNEL_PCI_H
#define METALOS_KERNEL_PCI_H

#include <stdint.h>

// PCI Configuration Space Registers
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

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

// PCI Functions
void pci_init(void);
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id);
void pci_enable_bus_mastering(pci_device_t* dev);

#endif // METALOS_KERNEL_PCI_H
