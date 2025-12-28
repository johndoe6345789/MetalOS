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

// PCIManager class implementation
PCIManager::PCIManager() : deviceCount(0) {}

uint32_t PCIManager::readConfig(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
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

void PCIManager::writeConfig(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
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

void PCIManager::probeDevice(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t vendorDevice = readConfig(bus, device, function, 0x00);
    uint16_t vendor_id = vendorDevice & 0xFFFF;
    uint16_t device_id = (vendorDevice >> 16) & 0xFFFF;
    
    // Check if device exists
    if (vendor_id == 0xFFFF) {
        return;
    }
    
    // Read class code
    uint32_t classRev = readConfig(bus, device, function, 0x08);
    uint8_t class_code = (classRev >> 24) & 0xFF;
    uint8_t subclass = (classRev >> 16) & 0xFF;
    uint8_t prog_if = (classRev >> 8) & 0xFF;
    uint8_t revision_id = classRev & 0xFF;
    
    // Store device info
    if (deviceCount < MAX_PCI_DEVICES) {
        PCIDevice* dev = &devices[deviceCount++];
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
            dev->bar[i] = readConfig(bus, device, function, 0x10 + (i * 4));
        }
    }
}

void PCIManager::init() {
    deviceCount = 0;
    
    // Scan all buses, devices, and functions
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            // Check if device exists (function 0)
            uint32_t vendorDevice = readConfig(bus, device, 0, 0x00);
            if ((vendorDevice & 0xFFFF) == 0xFFFF) {
                continue;  // Device doesn't exist
            }
            
            probeDevice(bus, device, 0);
            
            // Check if multi-function device
            uint32_t headerType = readConfig(bus, device, 0, 0x0C);
            if (headerType & 0x00800000) {
                // Multi-function device, scan other functions
                for (uint8_t function = 1; function < 8; function++) {
                    vendorDevice = readConfig(bus, device, function, 0x00);
                    if ((vendorDevice & 0xFFFF) != 0xFFFF) {
                        probeDevice(bus, device, function);
                    }
                }
            }
        }
    }
}

PCIDevice* PCIManager::findDevice(uint16_t vendor_id, uint16_t device_id) {
    for (uint32_t i = 0; i < deviceCount; i++) {
        if (devices[i].vendor_id == vendor_id && devices[i].device_id == device_id) {
            return &devices[i];
        }
    }
    return nullptr;
}

void PCIManager::enableBusMastering(PCIDevice* dev) {
    if (!dev) return;
    
    // Read command register (offset 0x04)
    uint32_t command = readConfig(dev->bus, dev->device, dev->function, 0x04);
    
    // Set bus master bit (bit 2)
    command |= 0x04;
    
    // Write back
    writeConfig(dev->bus, dev->device, dev->function, 0x04, command);
}

// Global PCI manager instance
static PCIManager globalPCI;

// C-compatible wrapper functions
extern "C" {

void pci_init(void) {
    globalPCI.init();
}

uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return globalPCI.readConfig(bus, device, function, offset);
}

void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    globalPCI.writeConfig(bus, device, function, offset, value);
}

pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id) {
    return reinterpret_cast<pci_device_t*>(globalPCI.findDevice(vendor_id, device_id));
}

void pci_enable_bus_mastering(pci_device_t* dev) {
    globalPCI.enableBusMastering(reinterpret_cast<PCIDevice*>(dev));
}

} // extern "C"
