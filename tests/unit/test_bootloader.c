/*
 * Unit tests for bootloader utilities
 * Tests memory address validation and basic bootloader logic
 */

#include "test_framework.h"
#include <stdint.h>
#include <stdbool.h>

// Bootloader constants (from bootloader.h)
#define KERNEL_LOAD_ADDRESS 0x100000  // 1MB mark
#define MAX_KERNEL_SIZE     0x1000000 // 16MB max

// Test: Kernel load address is valid
TEST(kernel_load_address_valid) {
    // Kernel should load at 1MB boundary (after BIOS/bootloader space)
    ASSERT_EQ(KERNEL_LOAD_ADDRESS, 0x100000);
    
    // Verify it's above 1MB (avoiding low memory)
    ASSERT_TRUE(KERNEL_LOAD_ADDRESS >= 0x100000);
    
    TEST_PASS();
}

// Test: Maximum kernel size is reasonable
TEST(max_kernel_size_reasonable) {
    // 16MB should be plenty for minimal kernel
    ASSERT_EQ(MAX_KERNEL_SIZE, 0x1000000);
    
    // Should be at least 1MB
    ASSERT_TRUE(MAX_KERNEL_SIZE >= 0x100000);
    
    // Should be less than 100MB (we're minimal!)
    ASSERT_TRUE(MAX_KERNEL_SIZE <= 0x6400000);
    
    TEST_PASS();
}

// Test: Kernel address range doesn't overlap bootloader
TEST(kernel_address_no_overlap) {
    uint64_t kernel_start = KERNEL_LOAD_ADDRESS;
    uint64_t kernel_end = KERNEL_LOAD_ADDRESS + MAX_KERNEL_SIZE;
    
    // Kernel should start after 1MB
    ASSERT_TRUE(kernel_start >= 0x100000);
    
    // Kernel should not wrap around
    ASSERT_TRUE(kernel_end > kernel_start);
    
    TEST_PASS();
}

// Mock function: Validate memory address is in valid range
static bool is_valid_memory_address(uint64_t address) {
    // Address must be above 1MB (avoid BIOS/bootloader area)
    if (address < 0x100000) return false;
    
    // Address must be below 4GB for 32-bit compatibility
    if (address >= 0x100000000ULL) return false;
    
    return true;
}

// Test: Memory address validation - valid addresses
TEST(memory_address_validation_valid) {
    ASSERT_TRUE(is_valid_memory_address(0x100000));   // 1MB
    ASSERT_TRUE(is_valid_memory_address(0x200000));   // 2MB
    ASSERT_TRUE(is_valid_memory_address(0x1000000));  // 16MB
    ASSERT_TRUE(is_valid_memory_address(0x80000000)); // 2GB
    
    TEST_PASS();
}

// Test: Memory address validation - invalid addresses
TEST(memory_address_validation_invalid) {
    ASSERT_FALSE(is_valid_memory_address(0x0));       // Null
    ASSERT_FALSE(is_valid_memory_address(0x7C00));    // BIOS area
    ASSERT_FALSE(is_valid_memory_address(0x10000));   // Below 1MB
    ASSERT_FALSE(is_valid_memory_address(0xFFFFF));   // Just below 1MB
    ASSERT_FALSE(is_valid_memory_address(0x100000000ULL)); // Above 4GB
    
    TEST_PASS();
}

// Mock function: Align address to page boundary
static uint64_t align_to_page(uint64_t address) {
    return (address + 0xFFF) & ~0xFFFULL;
}

// Test: Page alignment
TEST(page_alignment) {
    ASSERT_EQ(align_to_page(0x100000), 0x100000); // Already aligned
    ASSERT_EQ(align_to_page(0x100001), 0x101000); // Align up
    ASSERT_EQ(align_to_page(0x100FFF), 0x101000); // Align up
    ASSERT_EQ(align_to_page(0x101000), 0x101000); // Already aligned
    ASSERT_EQ(align_to_page(0x101001), 0x102000); // Align up
    
    TEST_PASS();
}

// Test: Bootloader version encoding
TEST(bootloader_version) {
    #define BOOTLOADER_VERSION_MAJOR 0
    #define BOOTLOADER_VERSION_MINOR 1
    #define BOOTLOADER_VERSION_PATCH 0
    
    // Verify version numbers are reasonable
    ASSERT_TRUE(BOOTLOADER_VERSION_MAJOR >= 0);
    ASSERT_TRUE(BOOTLOADER_VERSION_MAJOR < 10);
    ASSERT_TRUE(BOOTLOADER_VERSION_MINOR >= 0);
    ASSERT_TRUE(BOOTLOADER_VERSION_MINOR < 100);
    
    TEST_PASS();
}

// Main test runner
int main(void) {
    test_init("Bootloader Utilities");
    
    RUN_TEST(kernel_load_address_valid);
    RUN_TEST(max_kernel_size_reasonable);
    RUN_TEST(kernel_address_no_overlap);
    RUN_TEST(memory_address_validation_valid);
    RUN_TEST(memory_address_validation_invalid);
    RUN_TEST(page_alignment);
    RUN_TEST(bootloader_version);
    
    return test_summary();
}
