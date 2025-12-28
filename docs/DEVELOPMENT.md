# MetalOS Development Guide

## Getting Started

### Repository Structure

```
MetalOS/
├── bootloader/          # UEFI bootloader code
│   ├── src/            # Bootloader source
│   ├── include/        # Bootloader headers
│   └── Makefile        # Bootloader build
├── kernel/             # MetalOS kernel
│   ├── src/            # Kernel source
│   │   ├── core/      # Core kernel (memory, scheduler)
│   │   ├── hal/       # Hardware abstraction layer
│   │   ├── drivers/   # Device drivers
│   │   └── syscall/   # System call interface
│   ├── include/        # Kernel headers
│   └── Makefile        # Kernel build
├── userspace/          # User space components
│   ├── runtime/        # C/C++ runtime
│   ├── init/          # Init process
│   └── apps/          # Applications (QT6 hello world)
├── drivers/            # Additional drivers
│   ├── gpu/           # GPU drivers (Radeon RX 6600)
│   ├── input/         # Input device drivers
│   └── pci/           # PCI bus drivers
├── docs/              # Documentation
├── scripts/           # Build and utility scripts
├── tests/             # Test suite
└── tools/             # Development tools
```

## Coding Standards

### C/C++ Style

- **Indentation**: 4 spaces, no tabs
- **Naming**:
  - Functions: `snake_case`
  - Types: `PascalCase`
  - Constants: `UPPER_SNAKE_CASE`
  - Variables: `snake_case`
- **Comments**: Use `//` for single-line, `/* */` for multi-line
- **Headers**: Include guards with `METALOS_<PATH>_<FILE>_H`

Example:
```c
#ifndef METALOS_KERNEL_MEMORY_H
#define METALOS_KERNEL_MEMORY_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define KERNEL_BASE 0xFFFF800000000000

typedef struct {
    uint64_t physical_addr;
    uint64_t virtual_addr;
    uint64_t size;
} MemoryRegion;

// Initialize memory management subsystem
int memory_init(void);

// Allocate physical page
void* allocate_page(void);

#endif // METALOS_KERNEL_MEMORY_H
```

### Assembly Style

- **Syntax**: Intel syntax preferred
- **Comments**: Explain non-obvious operations
- **Labels**: Descriptive names

Example:
```nasm
; Set up initial GDT
setup_gdt:
    mov rax, gdt_pointer
    lgdt [rax]
    ret
```

## Development Workflow

### 1. Create Feature Branch
```bash
git checkout -b feature/my-feature
```

### 2. Make Changes
- Follow coding standards
- Add comments for complex logic
- Update documentation

### 3. Build and Test
```bash
make clean
make
make qemu
```

### 4. Commit Changes
```bash
git add .
git commit -m "Add feature: description"
```

### 5. Submit Pull Request
- Describe changes clearly
- Reference any related issues
- Ensure CI passes

## Debugging

### Serial Console Output

Add serial debugging output:
```c
#include <kernel/serial.h>

void my_function(void) {
    serial_printf("Debug: value=%d\n", some_value);
}
```

### QEMU Monitor

Access QEMU monitor during execution:
- Press `Ctrl+Alt+2` to switch to monitor
- Press `Ctrl+Alt+1` to return to guest
- Use `info registers` to inspect CPU state

### GDB Debugging

```bash
# Terminal 1: Start QEMU with GDB
make qemu-gdb

# Terminal 2: Connect GDB
gdb kernel/metalos.bin
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

Useful GDB commands:
- `break <function>` - Set breakpoint
- `continue` - Resume execution
- `step` - Step one instruction
- `next` - Step over function
- `info registers` - Show CPU registers
- `x/10x <addr>` - Examine memory
- `backtrace` - Show call stack

## Testing

### Unit Tests

Run kernel unit tests:
```bash
cd tests
make test
```

### Integration Tests

Test full system boot:
```bash
make test-integration
```

### Hardware Tests

Test on real hardware (advanced):
```bash
make usb-image
# Then boot from USB on test machine
```

## Common Tasks

### Adding a New Driver

1. Create driver file: `kernel/src/drivers/mydriver.c`
2. Create header: `kernel/include/drivers/mydriver.h`
3. Add to build: Edit `kernel/Makefile`
4. Initialize in kernel: Call from `kernel_main()`

### Adding System Call

1. Define syscall: `kernel/include/syscall/syscall.h`
2. Implement handler: `kernel/src/syscall/handlers.c`
3. Add to syscall table: `kernel/src/syscall/table.c`
4. Update userspace wrapper: `userspace/runtime/syscall.c`

### Modifying Memory Layout

1. Update linker script: `kernel/linker.ld`
2. Update memory map: `kernel/src/core/memory.c`
3. Update documentation: `docs/ARCHITECTURE.md`

## Performance Profiling

### Boot Time Analysis

```bash
# Enable boot time logging
make DEBUG=1 ENABLE_BOOTTIME=1
make qemu
# Check serial output for timing information
```

### CPU Profiling

Use QEMU's built-in profiling:
```bash
qemu-system-x86_64 -enable-kvm -cpu host \
    -d cpu_reset -D qemu.log \
    -drive file=build/metalos.img,format=raw
```

## Continuous Integration

Our CI pipeline:
1. **Build Check**: Ensure code compiles
2. **Unit Tests**: Run test suite
3. **Boot Test**: Verify kernel boots in QEMU
4. **Code Style**: Check formatting
5. **Documentation**: Verify docs build

## Resources

### Specifications
- [UEFI 2.10 Specification](https://uefi.org/specifications)
- [AMD64 Programmer's Manual](https://www.amd.com/en/support/tech-docs)
- [Intel SDM](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)

### OS Development
- [OSDev Wiki](https://wiki.osdev.org/)
- [Linux Kernel Source](https://kernel.org/) (for reference)
- [SerenityOS](https://github.com/SerenityOS/serenity) (good example)

### Graphics/GPU
- [AMD GPU Documentation](https://www.amd.com/en/support/kb/release-notes/rn-rad-win-crimson-16-12-1)
- [Linux DRM/KMS](https://dri.freedesktop.org/wiki/)

### QT6
- [QT6 Documentation](https://doc.qt.io/qt-6/)
- [QT Platform Abstraction](https://doc.qt.io/qt-6/qpa.html)

## Getting Help

- **Documentation**: Check `docs/` directory
- **Issues**: Search existing GitHub issues
- **Discussions**: Use GitHub Discussions for questions
- **IRC**: #metalos on libera.chat (planned)

## Contributing

We welcome contributions! Please:
1. Follow the coding standards
2. Write clear commit messages
3. Add tests for new features
4. Update documentation
5. Be respectful and constructive

See `CONTRIBUTING.md` for detailed guidelines.
