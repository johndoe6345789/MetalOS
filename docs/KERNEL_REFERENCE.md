# MetalOS Kernel - Quick Reference

## Kernel Modules Implemented

### Core System
- **GDT (gdt.c/h)**: Global Descriptor Table setup for x86_64
- **IDT (interrupts.c/h)**: Interrupt Descriptor Table with 32 exception handlers and IRQ support
- **Memory (memory.c/h)**: Physical memory manager and kernel heap allocator
- **PCI (pci.c/h)**: PCI bus enumeration and device configuration
- **Timer (timer.c/h)**: Programmable Interval Timer (PIT) with 1ms tick resolution

## Building the Kernel

### Quick Build
```bash
# With Ninja (fastest)
mkdir build && cd build
cmake -G Ninja ..
ninja kernel

# With Make
mkdir build && cd build
cmake ..
make kernel
```

### With Conan
```bash
conan install . --build=missing
conan build .
```

## Kernel API Reference

### Memory Management

```c
// Physical memory
void pmm_init(BootInfo* boot_info);
void* pmm_alloc_page(void);
void pmm_free_page(void* page);

// Kernel heap
void heap_init(void* start, size_t size);
void* kmalloc(size_t size);
void* kcalloc(size_t num, size_t size);
void kfree(void* ptr);  // Not implemented in bump allocator

// Utilities
void* memset(void* dest, int val, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int memcmp(const void* s1, const void* s2, size_t count);
```

### PCI Bus

```c
void pci_init(void);
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id);
void pci_enable_bus_mastering(pci_device_t* dev);
```

### Timer

```c
void timer_init(uint32_t frequency);  // Initialize with frequency in Hz
uint64_t timer_get_ticks(void);       // Get current tick count
void timer_wait(uint32_t ticks);      // Wait for specified ticks
```

### Interrupts

```c
void gdt_init(void);  // Initialize GDT
void idt_init(void);  // Initialize IDT and enable interrupts
void interrupt_handler(registers_t* regs);  // Generic handler
```

## Kernel Initialization Sequence

```c
void kernel_main(BootInfo* boot_info) {
    gdt_init();          // 1. Setup GDT
    idt_init();          // 2. Setup IDT and enable interrupts
    pmm_init(boot_info); // 3. Initialize physical memory
    heap_init(...);      // 4. Initialize kernel heap
    timer_init(1000);    // 5. Setup timer (1000Hz = 1ms per tick)
    pci_init();          // 6. Scan PCI bus
    // ... continue with GPU and input setup
}
```

## Memory Layout

```
0x0000000000000000 - 0x0000000000000FFF : NULL guard page
0x0000000000001000 - 0x00000000000FFFFF : Bootloader (temporary)
0x0000000000100000 - 0x0000000000FFFFFF : Kernel code/data
0x0000000001000000 - 0x00000000FFFFFFFF : Physical memory pool
```

## PCI Device Finding Example

```c
// Find AMD RX 6600 GPU
// Vendor: 0x1002 (AMD)
// Device: Check AMD documentation for exact device ID
pci_device_t* gpu = pci_find_device(0x1002, device_id);
if (gpu) {
    pci_enable_bus_mastering(gpu);
    // Access BAR0 for MMIO
    uint64_t mmio_base = gpu->bar[0] & ~0xF;
    // ... initialize GPU
}
```

## Interrupt Handling

### Exception Handlers (ISR 0-31)
- ISR 0: Divide by zero
- ISR 6: Invalid opcode  
- ISR 13: General protection fault
- ISR 14: Page fault
- ... (see interrupts.c for full list)

### IRQ Handlers (ISR 32-47)
- IRQ 0 (ISR 32): Timer interrupt
- IRQ 1 (ISR 33): Keyboard interrupt
- ... (add more as needed)

## Adding New Kernel Modules

1. Create header in `kernel/include/kernel/yourmodule.h`
2. Create implementation in `kernel/src/yourmodule.c`
3. Add to `kernel/CMakeLists.txt`:
   ```cmake
   set(KERNEL_C_SOURCES
       ...
       src/yourmodule.c
   )
   ```
4. Include in `kernel/src/main.c` and call init function
5. Rebuild: `ninja kernel` or `make kernel`

## Constants

```c
#define PAGE_SIZE 4096           // 4KB pages
#define TIMER_FREQUENCY 1000     // 1ms per tick
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
```

## Notes

- **No Console Module**: As requested, no console.c/h files are created
- **Minimal Design**: Only essential features for QT6 Hello World
- **Bump Allocator**: Current heap doesn't support freeing (upgrade later if needed)
- **Physical Memory**: Simple bitmap allocator (supports up to 64GB with 2MB bitmap)
- **PCI Scan**: Scans all 256 buses, 32 devices per bus
- **Timer**: Uses PIT in rate generator mode

## Testing

```bash
# Build and check symbols
cd build
ninja kernel
nm kernel/metalos.bin | grep -E "(init|handler)"

# Expected output shows all init functions
```

## Future Enhancements

- [ ] Implement proper heap allocator with free support
- [ ] Parse UEFI memory map properly in pmm_init
- [ ] Add keyboard interrupt handler
- [ ] Add GPU driver initialization
- [ ] Set up page tables for virtual memory
- [ ] Add input device drivers

## SMP (Multicore) API

### Core Detection and Management

```c
// Initialize SMP and start all CPU cores
void smp_init(void);

// Get number of online CPUs
uint8_t smp_get_cpu_count(void);

// Get current CPU ID (0 = BSP, 1+ = APs)
uint8_t smp_get_current_cpu(void);

// Check if multicore is enabled
bool smp_is_enabled(void);

// Get CPU information
cpu_info_t* smp_get_cpu_info(uint8_t cpu_id);
```

### APIC (Interrupt Controller)

```c
// Check if APIC is available
bool apic_is_available(void);

// Initialize Local APIC
void apic_init(void);

// Get Local APIC ID
uint8_t apic_get_id(void);

// Send End of Interrupt
void apic_send_eoi(void);

// Send Inter-Processor Interrupt
void apic_send_ipi(uint8_t dest_apic_id, uint8_t vector, uint32_t delivery_mode);
```

### Spinlocks

```c
spinlock_t lock;

// Initialize spinlock
void spinlock_init(spinlock_t* lock);

// Acquire lock (blocking)
void spinlock_acquire(spinlock_t* lock);

// Try to acquire (non-blocking)
bool spinlock_try_acquire(spinlock_t* lock);

// Release lock
void spinlock_release(spinlock_t* lock);

// Check if locked
bool spinlock_is_locked(spinlock_t* lock);
```

### Example: Protected Critical Section

```c
// Initialize lock once
static spinlock_t my_lock;
spinlock_init(&my_lock);

// Use in critical section
void update_shared_data(void) {
    spinlock_acquire(&my_lock);
    
    // Protected code here
    // Safe across all CPUs
    
    spinlock_release(&my_lock);
}
```

### Multicore-Aware Initialization

```c
void kernel_main(BootInfo* boot_info) {
    gdt_init();
    idt_init();
    pmm_init(boot_info);
    heap_init(...);
    timer_init(1000);
    pci_init();
    
    // Start all CPU cores
    smp_init();
    
    uint8_t num_cpus = smp_get_cpu_count();
    // num_cpus = 12 on 6-core/12-thread system
    
    // Continue with single-threaded initialization
    // (APs are idle, waiting for work)
}
```

## Multicore Support

MetalOS now supports multicore processors with up to 16 logical CPUs. Features:
- Automatic CPU detection
- APIC-based interrupt handling  
- Spinlocks for synchronization
- Per-CPU data structures
- Falls back to single-core if APIC unavailable

See [SMP_MULTICORE.md](SMP_MULTICORE.md) for detailed documentation.
