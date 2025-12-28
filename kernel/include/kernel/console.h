#ifndef METALOS_KERNEL_CONSOLE_H
#define METALOS_KERNEL_CONSOLE_H

#include <stdint.h>

// Simple framebuffer console for early boot messages

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

// Initialize console with framebuffer
void console_init(uint32_t* fb, uint32_t width, uint32_t height, uint32_t pitch);

// Print functions
void console_putchar(char c);
void console_print(const char* str);
void console_println(const char* str);
void console_clear(void);

// Set colors (RGB)
void console_set_color(uint32_t fg, uint32_t bg);

#endif // METALOS_KERNEL_CONSOLE_H
