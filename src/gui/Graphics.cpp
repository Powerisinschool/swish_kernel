#include <gui/Graphics.h>
#include <gui/Font.h>

#include "string.h"
#include "kernel/memory.hpp"

static uint32_t *framebuffer;
static size_t width, height, pitch;
static uint32_t *terminal_buffer;
static uint32_t *local_buffer;
static Color bg;

Color::Color() : r(255), g(255), b(255), a(255) {}

Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) : r(r), g(g), b(b), a(a) {}

Color::Color(const uint32_t color) {
    // ARGB format
    b = color & 0xFF;
    g = (color >> 8) & 0xFF;
    r = (color >> 16) & 0xFF;
    a = (color >> 24) & 0xFF;
}

Color Color::white = Color(255, 255, 255, 255);

uint32_t Color::get_bytes() const {
    return static_cast<uint32_t>(b) |
          (static_cast<uint32_t>(g) << 8) |
          (static_cast<uint32_t>(r) << 16) |
          (static_cast<uint32_t>(a) << 24);
}

namespace Graphics {
    void initialize(const limine_framebuffer *fb) {
        framebuffer = static_cast<uint32_t *>(fb->address);
        width = fb->width;
        height = fb->height;
        pitch = fb->pitch;

        local_buffer = static_cast<uint32_t *>(kmalloc(pitch * height));
        memset(local_buffer, 0xFF, pitch * height);
        terminal_buffer = static_cast<uint32_t *>(kmalloc(pitch * height));
        memset(terminal_buffer, 0, pitch * height);
    }

    void swap_buffers(const bool is_gui) {
        memcpy(framebuffer, is_gui ? local_buffer : terminal_buffer, pitch * height);
    }

    void set_bg(const Color color) {
        bg = color;
    }

    Color get_bg() {
        return bg;
    }

    void draw_pixel(const uint32_t x, const uint32_t y, const Color color) {
        if (x >= width || y >= height) {
            return;
        }
        auto* pixel_address = reinterpret_cast<uint32_t *>(reinterpret_cast<uintptr_t>(local_buffer) + (y * pitch) + (x * sizeof(uint32_t)));
        *pixel_address = color.get_bytes();
    }

    void draw_rect_filled(const uint32_t x, const uint32_t y, const size_t w, const size_t h, const Color color) {
        const uint32_t end_x = (x + w > width) ? width : x + w;
        const uint32_t end_y = (y + h > height) ? height : y + h;

        for (size_t i = y; i < end_y; i++) {
            for (size_t j = x; j < end_x; j++) {
                draw_pixel(j, i, color);
            }
        }
    }

    void draw_char(const uint32_t x, const uint32_t y, char c, const Color color) {
        if (c < 32 || c > 126) {
            c = ' ';
        }

        const int index = c - 32;

        for (uint32_t cx = 0; cx < 8; cx++) {
            const uint8_t top_byte = font8x16[index][cx];
            const uint8_t bottom_byte = font8x16[index][cx + 8];
            for (uint32_t cy = 0; cy < 8; cy++) {
                if (top_byte & (1 << cy)) {
                    draw_pixel(x + cx, y + cy, color);
                } else {
                    draw_pixel(x + cx, y + cy, bg);
                }
                if (bottom_byte & (1 << cy)) {
                    draw_pixel(x + cx, y + cy + 8, color);
                } else {
                    draw_pixel(x + cx, y + cy + 8, bg);
                }
            }
        }
    }

    void draw_string(const uint32_t x, const uint32_t y, const char *str, const Color color) {
        uint32_t current_x = x;
        uint32_t current_y = y;

        for (size_t i = 0; str[i] != '\0'; i++) {
            if (str[i] == '\n') {
                current_x = x;
                current_y += 16; // Move down by the font height
                continue;
            }

            draw_char(current_x, current_y, str[i], color);

            current_x += 8;
        }
    }

    uint32_t *get_terminal_buffer() {
        return terminal_buffer;
    }
}
