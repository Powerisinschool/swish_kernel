#include <gui/Window.h>

#include "string.h"
#include "kernel/memory.hpp"

Window::Window(const size_t x, const size_t y, const size_t width, const size_t height) : x(x), y(y), width(width), height(height) {
    pixel_buffer = static_cast<uint32_t *>(kmalloc(width * height * sizeof(uint32_t)));

    for (size_t i = 0; i < width * height; i++) {
        pixel_buffer[i] = Color{50, 50, 50, 255}.get_bytes();
    }
}

void Window::render() const {
    for (size_t row = 0; row < height; row++) {

        // Ask the Graphics layer for the exact screen memory address for this row
        uint32_t *screen_row_ptr = Graphics::get_buffer_address(x, y + row);

        // If the row is off the bottom of the screen, skip it to prevent a crash
        if (!screen_row_ptr) continue;

        // Calculate the starting address of the current row in our private buffer
        const uint32_t *window_row_ptr = pixel_buffer + (row * width);

        // Copy exactly one row of pixels
        memcpy(screen_row_ptr, window_row_ptr, width * sizeof(uint32_t));
    }
}
