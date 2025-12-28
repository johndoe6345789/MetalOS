#ifndef METALOS_KERNEL_PCI_H
#define METALOS_KERNEL_PCI_H

#include <stdint.h>

// PCI Configuration Space Registers
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// Maximum devices we'll track
#define MAX_PCI_DEVICES 256

/**
 * @struct pci_device_t
 * @brief Represents a PCI device discovered during bus enumeration
 * 
 * PCI (Peripheral Component Interconnect) is the standard bus for connecting
 * hardware devices like GPUs, network cards, storage controllers, etc.
 * 
 * Each PCI device is identified by:
 * - Bus, device, function numbers (BDF): Physical location
 * - Vendor ID and Device ID: Identifies manufacturer and model
 * - Class code, subclass, prog_if: Type of device (GPU, storage, etc.)
 * - Base Address Registers (BARs): Memory/IO regions used by device
 * 
 * Example vendor IDs:
 * - 0x1002: AMD/ATI (used by Radeon GPUs)
 * - 0x10DE: NVIDIA
 * - 0x8086: Intel
 */
typedef struct {
    uint8_t bus;         ///< PCI bus number (0-255)
    uint8_t device;      ///< Device number on bus (0-31)
    uint8_t function;    ///< Function number within device (0-7)
    uint16_t vendor_id;  ///< Vendor identifier (e.g., 0x1002 for AMD)
    uint16_t device_id;  ///< Device identifier (specific model)
    uint8_t class_code;  ///< Device class (0x03 = display controller)
    uint8_t subclass;    ///< Device subclass (0x00 = VGA compatible)
    uint8_t prog_if;     ///< Programming interface
    uint8_t revision_id; ///< Device revision
    uint32_t bar[6];     ///< Base Address Registers (memory/IO regions)
} pci_device_t;

#ifdef __cplusplus
/**
 * @struct PCIDevice
 * @brief C++ equivalent of pci_device_t for type compatibility
 */
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

/**
 * @class PCIManager
 * @brief Manages PCI bus enumeration and device configuration
 * 
 * The PCIManager scans the PCI bus hierarchy to discover all connected devices.
 * PCI configuration is done through two I/O ports:
 * - 0xCF8 (CONFIG_ADDRESS): Specifies which device and register to access
 * - 0xCFC (CONFIG_DATA): Reads/writes the configuration register
 * 
 * PCI devices are organized hierarchically:
 * - Up to 256 buses
 * - Up to 32 devices per bus
 * - Up to 8 functions per device (most devices have only function 0)
 * 
 * Each device has 256 bytes of configuration space containing:
 * - Device identification (vendor/device ID)
 * - Command and status registers
 * - Base Address Registers (BARs) for memory-mapped I/O
 * - Interrupt configuration
 * - Device-specific registers
 */
class PCIManager {
private:
    PCIDevice devices[MAX_PCI_DEVICES];  ///< Array of discovered devices
    uint32_t deviceCount;                ///< Number of devices found
    
    /**
     * @brief Probe a specific PCI function and add to device list
     * @param bus Bus number
     * @param device Device number
     * @param function Function number
     * @note Reads device identification and stores in devices array
     */
    void probeDevice(uint8_t bus, uint8_t device, uint8_t function);

public:
    /** @brief Constructor - initializes device count to 0 */
    PCIManager();
    
    /**
     * @brief Scan all PCI buses and enumerate devices
     * @note Scans all 256 buses, 32 devices per bus, up to 8 functions per device
     * @note Skips non-existent devices (vendor ID 0xFFFF)
     */
    void init();
    
    /**
     * @brief Read a 32-bit value from PCI configuration space
     * @param bus Bus number
     * @param device Device number
     * @param function Function number
     * @param offset Register offset (must be 4-byte aligned)
     * @return 32-bit configuration register value
     */
    uint32_t readConfig(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
    
    /**
     * @brief Write a 32-bit value to PCI configuration space
     * @param bus Bus number
     * @param device Device number
     * @param function Function number
     * @param offset Register offset (must be 4-byte aligned)
     * @param value Value to write
     */
    void writeConfig(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
    
    /**
     * @brief Find a PCI device by vendor and device ID
     * @param vendor_id Vendor identifier (e.g., 0x1002 for AMD)
     * @param device_id Device identifier
     * @return Pointer to PCIDevice if found, nullptr if not found
     * @note Useful for finding specific hardware (e.g., "find AMD RX 6600 GPU")
     */
    PCIDevice* findDevice(uint16_t vendor_id, uint16_t device_id);
    
    /**
     * @brief Enable bus mastering for a PCI device
     * @param dev Pointer to PCI device
     * @note Bus mastering allows device to perform DMA (Direct Memory Access)
     * @note Required for most devices to function properly
     */
    void enableBusMastering(PCIDevice* dev);
};

extern "C" {
#endif

/* PCI - C-compatible interface */

/**
 * @brief Initialize PCI subsystem and enumerate all devices
 * @note Should be called during kernel initialization
 */
void pci_init(void);

/**
 * @brief Read from PCI configuration space
 * @param bus Bus number
 * @param device Device number
 * @param function Function number
 * @param offset Register offset
 * @return 32-bit register value
 */
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

/**
 * @brief Write to PCI configuration space
 * @param bus Bus number
 * @param device Device number
 * @param function Function number
 * @param offset Register offset
 * @param value Value to write
 */
void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);

/**
 * @brief Find a PCI device by vendor and device ID
 * @param vendor_id Vendor identifier
 * @param device_id Device identifier
 * @return Pointer to device structure if found, NULL otherwise
 */
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id);

/**
 * @brief Enable bus mastering for a PCI device
 * @param dev Pointer to PCI device structure
 */
void pci_enable_bus_mastering(pci_device_t* dev);

#ifdef __cplusplus
}
#endif

#endif // METALOS_KERNEL_PCI_H
