/*
 * Unit tests for console module
 * Tests console initialization, color setting, and basic operations
 */

#include "test_framework.h"
#include <stdint.h>
#include <string.h>

// Mock console structure (matching kernel/console.h)
typedef struct {
    uint32_t* framebuffer;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t x;
    uint32_t y;
    uint32_t fg_color;
    uint32_t bg_color;
} Console;

// Mock framebuffer for testing (using smaller size to avoid stack issues)
static uint32_t test_framebuffer[800 * 600];

// Simplified console functions for unit testing
// In real implementation, these would be in a testable module
static Console test_console;

void mock_console_init(uint32_t* fb, uint32_t width, uint32_t height, uint32_t pitch) {
    test_console.framebuffer = fb;
    test_console.width = width;
    test_console.height = height;
    test_console.pitch = pitch;
    test_console.x = 0;
    test_console.y = 0;
    test_console.fg_color = 0xFFFFFFFF; // White
    test_console.bg_color = 0x00000000; // Black
}

void mock_console_set_color(uint32_t fg, uint32_t bg) {
    test_console.fg_color = fg;
    test_console.bg_color = bg;
}

void mock_console_clear(void) {
    if (!test_console.framebuffer) return;
    
    for (uint32_t y = 0; y < test_console.height; y++) {
        for (uint32_t x = 0; x < test_console.width; x++) {
            test_console.framebuffer[y * (test_console.pitch / 4) + x] = test_console.bg_color;
        }
    }
    
    test_console.x = 0;
    test_console.y = 0;
}

// Test: Console initialization
TEST(console_init) {
    mock_console_init(test_framebuffer, 1920, 1080, 1920 * 4);
    
    ASSERT_NOT_NULL(test_console.framebuffer);
    ASSERT_EQ(test_console.width, 1920);
    ASSERT_EQ(test_console.height, 1080);
    ASSERT_EQ(test_console.pitch, 1920 * 4);
    ASSERT_EQ(test_console.x, 0);
    ASSERT_EQ(test_console.y, 0);
    ASSERT_EQ(test_console.fg_color, 0xFFFFFFFF);
    ASSERT_EQ(test_console.bg_color, 0x00000000);
    
    TEST_PASS();
}

// Test: Console color setting
TEST(console_set_color) {
    mock_console_init(test_framebuffer, 1920, 1080, 1920 * 4);
    
    mock_console_set_color(0xFF0000FF, 0x00FF00FF);
    
    ASSERT_EQ(test_console.fg_color, 0xFF0000FF);
    ASSERT_EQ(test_console.bg_color, 0x00FF00FF);
    
    TEST_PASS();
}

// Test: Console clear operation
TEST(console_clear) {
    mock_console_init(test_framebuffer, 800, 600, 800 * 4);
    mock_console_set_color(0xFFFFFFFF, 0x00112233);
    
    // Set some position
    test_console.x = 100;
    test_console.y = 200;
    
    mock_console_clear();
    
    // Check that position is reset
    ASSERT_EQ(test_console.x, 0);
    ASSERT_EQ(test_console.y, 0);
    
    // Check that first few pixels are background color
    ASSERT_EQ(test_framebuffer[0], 0x00112233);
    ASSERT_EQ(test_framebuffer[1], 0x00112233);
    ASSERT_EQ(test_framebuffer[10], 0x00112233);
    
    TEST_PASS();
}

// Test: Console with NULL framebuffer (edge case)
TEST(console_null_framebuffer) {
    mock_console_init(NULL, 1920, 1080, 1920 * 4);
    
    ASSERT_NULL(test_console.framebuffer);
    
    // Clear should handle NULL gracefully (no crash)
    mock_console_clear();
    
    TEST_PASS();
}

// Test: Console with small dimensions
TEST(console_small_dimensions) {
    mock_console_init(test_framebuffer, 64, 48, 64 * 4);
    
    ASSERT_EQ(test_console.width, 64);
    ASSERT_EQ(test_console.height, 48);
    
    TEST_PASS();
}

// Main test runner
int main(void) {
    test_init("Console Module");
    
    RUN_TEST(console_init);
    RUN_TEST(console_set_color);
    RUN_TEST(console_clear);
    RUN_TEST(console_null_framebuffer);
    RUN_TEST(console_small_dimensions);
    
    return test_summary();
}
