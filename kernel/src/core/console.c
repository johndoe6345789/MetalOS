/*
 * Simple framebuffer console for kernel messages
 * Minimal implementation - just enough for debugging
 */

#include "kernel/console.h"

// Simple 8x8 bitmap font (ASCII characters 32-126)
// This is a minimal font representation
static const uint8_t font_8x8[96][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // !
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // "
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // #
    // ... (simplified - would need full 96 characters)
    // For now, we'll render simple blocks for all chars
};

static Console console;

void console_init(uint32_t* fb, uint32_t width, uint32_t height, uint32_t pitch) {
    console.framebuffer = fb;
    console.width = width;
    console.height = height;
    console.pitch = pitch;
    console.x = 0;
    console.y = 0;
    console.fg_color = 0xFFFFFFFF; // White
    console.bg_color = 0x00000000; // Black
}

void console_clear(void) {
    if (!console.framebuffer) return;
    
    for (uint32_t y = 0; y < console.height; y++) {
        for (uint32_t x = 0; x < console.width; x++) {
            console.framebuffer[y * (console.pitch / 4) + x] = console.bg_color;
        }
    }
    
    console.x = 0;
    console.y = 0;
}

void console_set_color(uint32_t fg, uint32_t bg) {
    console.fg_color = fg;
    console.bg_color = bg;
}

// Draw a simple 8x8 character (simplified version)
static void draw_char(char c, uint32_t x, uint32_t y) {
    if (!console.framebuffer) return;
    if (x + 8 > console.width || y + 8 > console.height) return;
    
    // For simplicity, just draw a simple pattern based on character
    // In a real implementation, we'd use the font bitmap
    for (int cy = 0; cy < 8; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            uint32_t pixel_x = x + cx;
            uint32_t pixel_y = y + cy;
            
            // Simple algorithm: draw pixels based on char value
            // This creates a unique pattern for each character
            uint8_t pattern = (c + cy) & (1 << cx) ? 0xFF : 0x00;
            
            uint32_t color = pattern ? console.fg_color : console.bg_color;
            console.framebuffer[pixel_y * (console.pitch / 4) + pixel_x] = color;
        }
    }
}

void console_putchar(char c) {
    if (!console.framebuffer) return;
    
    if (c == '\n') {
        console.x = 0;
        console.y += 8;
        if (console.y >= console.height) {
            console.y = 0; // Wrap around (simplified scrolling)
        }
        return;
    }
    
    if (c == '\r') {
        console.x = 0;
        return;
    }
    
    draw_char(c, console.x, console.y);
    console.x += 8;
    
    if (console.x >= console.width) {
        console.x = 0;
        console.y += 8;
        if (console.y >= console.height) {
            console.y = 0; // Wrap around
        }
    }
}

void console_print(const char* str) {
    if (!str) return;
    
    while (*str) {
        console_putchar(*str);
        str++;
    }
}

void console_println(const char* str) {
    console_print(str);
    console_putchar('\n');
}
