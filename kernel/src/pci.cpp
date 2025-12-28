/**
 * @file pci.cpp
 * @brief Implementation of PCI bus enumeration and device management
 * 
 * PCI (Peripheral Component Interconnect) is the standard bus for connecting
 * hardware devices. This implementation scans the PCI bus to discover devices
 * and provides functions to configure them.
 */

#include "kernel/pci.h"
#include "kernel/memory.h"

/**
 * @brief Write a 32-bit value to an I/O port
 * @param port I/O port address
 * @param value 32-bit value to write
 */
static inline void outl(uint16_t port, uint32_t value) {
    __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

/**
 * @brief Read a 32-bit value from an I/O port
 * @param port I/O port address
 * @return 32-bit value read from port
 */
static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* PCIManager class implementation */

/**
 * @brief Constructor - initializes device count to zero
 */
PCIManager::PCIManager() : deviceCount(0) {}

/**
 * @brief Read a 32-bit value from PCI configuration space
 * 
 * PCI configuration space is accessed through two I/O ports:
 * - 0xCF8 (CONFIG_ADDRESS): Write the address of config register to read
 * - 0xCFC (CONFIG_DATA): Read the 32-bit value from that register
 * 
 * The address format (32 bits):
 * - Bit 31: Enable bit (must be 1)
 * - Bits 16-23: Bus number (0-255)
 * - Bits 11-15: Device number (0-31)
 * - Bits 8-10: Function number (0-7)
 * - Bits 0-7: Register offset (4-byte aligned, bits 0-1 ignored)
 * 
 * Each PCI device has 256 bytes of configuration space containing:
 * - Device identification (vendor/device ID at offset 0x00)
 * - Command/status registers (offset 0x04)
 * - Class code (offset 0x08)
 * - BARs (Base Address Registers at offsets 0x10-0x24)
 * - Interrupt configuration
 * 
 * @param bus Bus number (0-255)
 * @param device Device number on bus (0-31)
 * @param function Function number within device (0-7)
 * @param offset Register offset (must be 4-byte aligned)
 * @return 32-bit configuration register value
 */
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

/**
 * @brief Write a 32-bit value to PCI configuration space
 * 
 * Similar to readConfig, but writes a value to the specified register.
 * Used for device configuration, such as:
 * - Enabling bus mastering
 * - Enabling memory/IO space access
 * - Configuring interrupt lines
 * 
 * @param bus Bus number
 * @param device Device number
 * @param function Function number
 * @param offset Register offset (4-byte aligned)
 * @param value 32-bit value to write
 */
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

/**
 * @brief Probe a specific PCI device/function and add to device list
 * 
 * Reads device information from PCI configuration space and stores it:
 * - Vendor ID and Device ID (for identification)
 * - Class code, subclass, prog_if (device type)
 * - Revision ID
 * - All 6 Base Address Registers (BARs)
 * 
 * BARs specify memory or I/O regions used by the device:
 * - Bit 0: 0=memory BAR, 1=I/O BAR
 * - For memory BARs:
 *   - Bits 1-2: Type (00=32-bit, 10=64-bit)
 *   - Bit 3: Prefetchable
 *   - Bits 4-31: Base address (4KB aligned)
 * 
 * @param bus Bus number
 * @param device Device number
 * @param function Function number
 */
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

/**
 * @brief Initialize PCI subsystem by scanning all buses
 * 
 * Performs a complete scan of the PCI bus hierarchy:
 * - Iterates through all 256 possible buses
 * - For each bus, checks all 32 device slots
 * - For each device, checks if it's multi-function
 * - If multi-function, scans all 8 functions
 * 
 * A device exists if its vendor ID is not 0xFFFF. The header type
 * register (offset 0x0C) has bit 7 set for multi-function devices.
 * 
 * This approach is brute-force but simple and reliable. More sophisticated
 * implementations would parse ACPI tables to find PCI buses.
 */
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

/**
 * @brief Find a PCI device by vendor and device ID
 * 
 * Searches through the list of discovered devices for a match.
 * Useful for finding specific hardware, e.g.:
 * - AMD RX 6600: vendor=0x1002, device=0x73FF
 * - Intel NIC: vendor=0x8086, device=various
 * 
 * @param vendor_id Vendor identifier (e.g., 0x1002 for AMD)
 * @param device_id Device identifier (specific model)
 * @return Pointer to PCIDevice if found, nullptr otherwise
 */
PCIDevice* PCIManager::findDevice(uint16_t vendor_id, uint16_t device_id) {
    for (uint32_t i = 0; i < deviceCount; i++) {
        if (devices[i].vendor_id == vendor_id && devices[i].device_id == device_id) {
            return &devices[i];
        }
    }
    return nullptr;
}

/**
 * @brief Enable bus mastering for a PCI device
 * 
 * Bus mastering allows a device to perform DMA (Direct Memory Access) -
 * reading and writing system memory without CPU involvement. This is
 * essential for high-performance devices like GPUs and network cards.
 * 
 * The command register (offset 0x04) contains control bits:
 * - Bit 0: I/O Space Enable
 * - Bit 1: Memory Space Enable
 * - Bit 2: Bus Master Enable ← We set this bit
 * - Bit 10: Interrupt Disable
 * 
 * @param dev Pointer to PCI device structure
 */
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
