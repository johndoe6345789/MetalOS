# Testing MetalOS

This document describes the testing infrastructure for MetalOS.

## Philosophy

Following MetalOS's minimalism philosophy, our testing approach is:

- **Minimal dependencies** - Custom lightweight test framework, no external libraries
- **Focused tests** - Test only what can be tested in isolation
- **Practical** - QEMU integration tests for full system validation
- **Fast** - Quick feedback loop for developers

## Test Types

### 1. Unit Tests

Unit tests verify individual components in isolation.

**Location**: `tests/unit/`

**Running unit tests**:
```bash
# Build and run all unit tests
make test

# Or run from tests directory
cd tests
make test

# Clean test artifacts
make clean
```

**Current test suites**:
- `test_console.c` - Console module tests (initialization, colors, clearing)
- `test_bootloader.c` - Bootloader utility tests (memory validation, alignment)

**Writing new tests**:

Create a new test file in `tests/unit/`:

```c
#include "test_framework.h"

TEST(my_test_name) {
    // Test setup
    int value = 42;
    
    // Assertions
    ASSERT_EQ(value, 42);
    ASSERT_TRUE(value > 0);
    ASSERT_NOT_NULL(&value);
    
    TEST_PASS();
}

int main(void) {
    test_init("My Test Suite");
    
    RUN_TEST(my_test_name);
    
    return test_summary();
}
```

**Available assertions**:
- `ASSERT_TRUE(condition)` - Assert condition is true
- `ASSERT_FALSE(condition)` - Assert condition is false
- `ASSERT_EQ(a, b)` - Assert a equals b
- `ASSERT_NE(a, b)` - Assert a not equals b
- `ASSERT_NULL(ptr)` - Assert pointer is NULL
- `ASSERT_NOT_NULL(ptr)` - Assert pointer is not NULL
- `ASSERT_STR_EQ(a, b)` - Assert strings are equal
- `TEST_PASS()` - Mark test as passed (call at end of successful test)

### 2. QEMU Integration Tests

QEMU tests verify the full system boots correctly in an emulated environment.

**Location**: `.github/workflows/qemu-test.yml`

**What it does**:
1. Builds bootloader and kernel
2. Creates bootable image
3. Launches QEMU with UEFI firmware
4. Waits for boot sequence (configurable timeout)
5. Captures screenshot of boot state
6. Captures serial output
7. Shuts down gracefully

**Artifacts**:
- Screenshot: `qemu-screenshot.png`
- Serial log: `serial.log`

These artifacts are uploaded to GitHub Actions for inspection.

**Running locally**:
```bash
# Build and test in QEMU
make qemu

# With debug output
make qemu-debug

# With GDB server
make qemu-gdb
```

## Test Framework

MetalOS uses a custom minimal test framework (`tests/include/test_framework.h`) with:

- No external dependencies
- Color-coded output
- Simple assertion macros
- Test statistics and summary

## Continuous Integration

All tests run automatically on:
- Push to `main` or `copilot/*` branches
- Pull requests to `main`
- Manual workflow dispatch

**Workflows**:
- `.github/workflows/unit-tests.yml` - Unit test suite
- `.github/workflows/qemu-test.yml` - QEMU boot test

## What We Don't Test

Following minimalism philosophy, we intentionally don't test:

❌ **Code coverage targets** - No arbitrary percentage goals  
❌ **Integration with external systems** - No networking tests  
❌ **Performance benchmarks** - Functionality over speed  
❌ **Fuzz testing** - Out of scope for demo OS  
❌ **Static analysis beyond basics** - Keep it simple

## Adding New Tests

When adding new functionality:

1. **Write unit tests** for testable, isolated functions
2. **Update QEMU test** if boot behavior changes
3. **Document test approach** in code comments
4. **Run tests locally** before committing

## Test Output Examples

**Successful unit test run**:
```
========================================
  MetalOS Test Suite: Console Module
========================================
[ RUN  ] console_init
[ PASS ] 
[ RUN  ] console_set_color
[ PASS ] 

========================================
  Total tests:  2
  Passed:       2
  Failed:       0
========================================

✓ All tests passed!
```

**Failed test**:
```
[ RUN  ] memory_validation
[FAILED] tests/unit/test_memory.c:42: Expected 1024, got 512

========================================
  Total tests:  1
  Passed:       0
  Failed:       1
========================================

✗ Some tests failed!
```

## Troubleshooting

**Tests won't compile**:
- Check that you have `gcc` and `make` installed
- Run `make clean` in tests directory
- Verify include paths in `tests/Makefile`

**QEMU test fails**:
- Check that QEMU and OVMF are installed
- Verify bootloader/kernel build successfully
- Check serial log artifact for boot errors

**Test hangs**:
- Increase timeout in test code
- Check for infinite loops in tested code
- Run with `make test-verbose` for more output

## Future Test Improvements

As MetalOS evolves, we may add:

- [ ] Memory allocator tests
- [ ] PCI enumeration tests (mocked)
- [ ] Input driver tests (mocked)
- [ ] GPU initialization tests (mocked)
- [ ] More comprehensive QEMU validation

Remember: **Test what matters, skip what doesn't.** Quality over coverage.
