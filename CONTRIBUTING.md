# Contributing to MetalOS

Thank you for your interest in contributing to MetalOS!

## Philosophy

MetalOS has a unique philosophy:

1. **Minimal by Design**: Every line of code must justify its existence
2. **Purpose-Built**: The goal is to run QT6 Hello World, not to be a general-purpose OS
3. **Creative Freedom**: We're not bound by POSIX or traditional OS designs
4. **Precise Drivers**: Hardware driver code must be accurate and follow specs

## What We Accept

### Welcome Contributions ✅

- Bug fixes in existing code
- Documentation improvements
- Build system enhancements
- Code simplification (making things simpler is better than adding features)
- Hardware driver improvements (especially GPU)
- QT6 port work
- Performance optimizations

### Think Twice Before Adding ❌

- New features that don't directly support QT6
- Additional hardware support beyond target (AMD64 + RX 6600)
- Complex abstractions
- POSIX compatibility layers
- Security features (this is a demo project)
- Multi-user support

**Golden Rule**: Ask yourself "Does this help run QT6 Hello World?" before adding anything.

## Code Style

### C Code

```c
// 4 spaces, no tabs
// snake_case for functions and variables
// PascalCase for types
// UPPER_SNAKE_CASE for constants

#ifndef METALOS_PATH_FILE_H
#define METALOS_PATH_FILE_H

#include <stdint.h>

#define MAX_BUFFER_SIZE 1024

typedef struct {
    uint64_t address;
    uint64_t size;
} MemoryBlock;

// Functions: descriptive names, clear purpose
int allocate_memory(uint64_t size);
void free_memory(void* ptr);

#endif // METALOS_PATH_FILE_H
```

### C++ Code (for applications)

```cpp
// 4 spaces, no tabs
// camelCase for methods
// PascalCase for classes

class DisplayManager {
private:
    uint32_t width;
    uint32_t height;
    
public:
    DisplayManager(uint32_t w, uint32_t h);
    void render();
};
```

### Assembly

```nasm
; Intel syntax
; Clear comments explaining what's happening
; Descriptive labels

section .text
global _start

_start:
    ; Set up stack
    mov rsp, stack_top
    
    ; Call kernel main
    call kernel_main
    
    ; Halt if we return
    cli
    hlt
```

## Submission Process

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/my-feature`
3. **Make your changes**
4. **Test thoroughly**: At minimum, test in QEMU
5. **Commit with clear messages**: `git commit -m "Add feature: description"`
6. **Push to your fork**: `git push origin feature/my-feature`
7. **Open a Pull Request**

## Pull Request Guidelines

### Good PR Description

```
## What does this do?

Implements basic PCI device enumeration in the HAL.

## Why is this needed?

Required to detect and initialize the Radeon RX 6600 GPU.

## How was it tested?

- Tested in QEMU with emulated PCI devices
- Verified PCI device list output
- Confirmed GPU is detected correctly

## Related Issues

Closes #42
```

### What We Look For

- **Clear purpose**: Why is this change needed?
- **Minimal scope**: Does it do one thing well?
- **Testing**: Has it been tested?
- **Documentation**: Are non-obvious parts explained?
- **No regressions**: Does existing functionality still work?

## Testing

### Minimum Testing

Every PR should be tested in QEMU:

```bash
mkdir build && cd build
cmake ..
cmake --build .
cmake --build . --target qemu
```

Or if you already have a build directory:

```bash
cd build
cmake --build . --target clean
cmake --build .
cmake --build . --target qemu
```

Verify:
- Code compiles without warnings
- System boots (if applicable)
- Changes work as expected
- No crashes or hangs

### Hardware Testing (if applicable)

For GPU driver changes or hardware-specific code, testing on real hardware is highly recommended.

## Documentation

If you change functionality, update relevant documentation:

- Code comments for complex logic
- README.md for user-facing changes
- docs/ARCHITECTURE.md for design changes
- docs/DEVELOPMENT.md for development workflow changes

## Questions?

- Open a GitHub Discussion for general questions
- Comment on existing Issues for specific topics
- Tag maintainers in PRs if you need feedback

## Code of Conduct

Be respectful, constructive, and helpful. This is a learning project - everyone is welcome regardless of experience level.

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (see LICENSE file).

---

Happy hacking! 🚀
