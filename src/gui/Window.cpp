#include <gui/Window.h>

#include "string.h"
#include "kernel/memory.hpp"

Window::Window(const size_t x, const size_t y, const size_t width, const size_t height) : x(x),
    y(y) {
    const auto pixel_buffer = static_cast<uint32_t *>(kmalloc(width * height * sizeof(uint32_t)));
    surface = new Surface{pixel_buffer, width, height, width * sizeof(uint32_t), Color::white};

    for (size_t i = 0; i < width * height; i++) {
        pixel_buffer[i] = Color::white.get_bytes();
    }
}

void Window::inject_key(const char c) {
    if (c == '\b') {
        if (char_index > 0) {
            text_buffer[--char_index] = ' ';
        }
    } else {
        if (char_index >= 31) return;
        text_buffer[char_index++] = c;
        text_buffer[char_index] = '\0';
    }
    Graphics::draw_string(surface, 10, 10, text_buffer, Color::black);
}

void Window::render() const {
    Graphics::draw_string(surface, 10, 10, text_buffer, Color::black);
    for (size_t row = 0; row < surface->height; row++) {
        uint32_t *screen_row_ptr = Graphics::get_buffer_address(x, y + row);
        if (!screen_row_ptr) continue;

        const uint32_t *window_row_ptr = surface->buffer + (row * surface->width);
        memcpy(screen_row_ptr, window_row_ptr, surface->width * sizeof(uint32_t));
    }
}
