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
    Graphics::draw_bg(surface);
    Graphics::draw_string(surface, 10, 10, text_buffer, Color::black);
    for (size_t row = 0; row < surface->height; row++) {
        uint32_t *screen_row_ptr = Graphics::get_buffer_address(x, y + row);
        if (!screen_row_ptr) continue;

        const uint32_t *window_row_ptr = surface->buffer + (row * surface->width);
        memcpy(screen_row_ptr, window_row_ptr, surface->width * sizeof(uint32_t));
    }
}

void Window::on_mouse_button(uint64_t local_x, uint64_t local_y, uint8_t button, const bool is_down) const {
    if (is_down) {
        Graphics::set_bg(surface, Color::red);
    } else {
        Graphics::set_bg(surface, Color::white);
    }
}

const Surface *Window::get_surface() const {
    return surface;
}
