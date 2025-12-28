/*
 * MetalOS Kernel - PCI Bus Support
 * 
 * Minimal PCI enumeration and configuration
 * Only what's needed to find and initialize the GPU
 */

#include "kernel/pci.h"
#include "kernel/memory.h"

// I/O port access functions
static inline void outl(uint16_t port, uint32_t value) {
    __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Maximum devices we'll track
#define MAX_PCI_DEVICES 256
static pci_device_t pci_devices[MAX_PCI_DEVICES];
static uint32_t pci_device_count = 0;

// Read from PCI configuration space
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (uint32_t)(
        ((uint32_t)bus << 16) |
        ((uint32_t)device << 11) |
        ((uint32_t)function << 8) |
        (offset & 0xFC) |
        0x80000000
    );
    
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

// Write to PCI configuration space
void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address = (uint32_t)(
        ((uint32_t)bus << 16) |
        ((uint32_t)device << 11) |
        ((uint32_t)function << 8) |
        (offset & 0xFC) |
        0x80000000
    );
    
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

// Probe a PCI device
static void pci_probe_device(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t vendor_device = pci_read_config(bus, device, function, 0x00);
    uint16_t vendor_id = vendor_device & 0xFFFF;
    uint16_t device_id = (vendor_device >> 16) & 0xFFFF;
    
    // Check if device exists
    if (vendor_id == 0xFFFF) {
        return;
    }
    
    // Read class code
    uint32_t class_rev = pci_read_config(bus, device, function, 0x08);
    uint8_t class_code = (class_rev >> 24) & 0xFF;
    uint8_t subclass = (class_rev >> 16) & 0xFF;
    uint8_t prog_if = (class_rev >> 8) & 0xFF;
    uint8_t revision_id = class_rev & 0xFF;
    
    // Store device info
    if (pci_device_count < MAX_PCI_DEVICES) {
        pci_device_t* dev = &pci_devices[pci_device_count++];
        dev->bus = bus;
        dev->device = device;
        dev->function = function;
        dev->vendor_id = vendor_id;
        dev->device_id = device_id;
        dev->class_code = class_code;
        dev->subclass = subclass;
        dev->prog_if = prog_if;
        dev->revision_id = revision_id;
        
        // Read BARs (Base Address Registers)
        for (int i = 0; i < 6; i++) {
            dev->bar[i] = pci_read_config(bus, device, function, 0x10 + (i * 4));
        }
    }
}

// Initialize PCI subsystem
void pci_init(void) {
    // Scan all buses, devices, and functions
    // Note: On real hardware, could optimize by stopping after consecutive empty buses
    for (uint16_t bus = 0; bus < 256; bus++) {
        uint8_t devices_found = 0;
        
        for (uint8_t device = 0; device < 32; device++) {
            // Check if device exists (function 0)
            uint32_t vendor_device = pci_read_config(bus, device, 0, 0x00);
            if ((vendor_device & 0xFFFF) == 0xFFFF) {
                continue;  // Device doesn't exist
            }
            
            devices_found++;
            pci_probe_device(bus, device, 0);
            
            // Check if multi-function device
            uint32_t header_type = pci_read_config(bus, device, 0, 0x0C);
            if (header_type & 0x00800000) {
                // Multi-function device, scan other functions
                for (uint8_t function = 1; function < 8; function++) {
                    vendor_device = pci_read_config(bus, device, function, 0x00);
                    if ((vendor_device & 0xFFFF) != 0xFFFF) {
                        pci_probe_device(bus, device, function);
                    }
                }
            }
        }
        
        // Early termination: if no devices found in this bus and we're past bus 0, 
        // we can potentially stop (though some systems have gaps)
        // For now, continue full scan for maximum compatibility
    }
}

// Find a PCI device by vendor and device ID
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id) {
    for (uint32_t i = 0; i < pci_device_count; i++) {
        if (pci_devices[i].vendor_id == vendor_id && 
            pci_devices[i].device_id == device_id) {
            return &pci_devices[i];
        }
    }
    return NULL;
}

// Enable bus mastering for a device
void pci_enable_bus_mastering(pci_device_t* dev) {
    if (!dev) return;
    
    // Read command register (offset 0x04)
    uint32_t command = pci_read_config(dev->bus, dev->device, dev->function, 0x04);
    
    // Set bus master bit (bit 2)
    command |= 0x04;
    
    // Write back
    pci_write_config(dev->bus, dev->device, dev->function, 0x04, command);
}
