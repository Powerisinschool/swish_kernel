#pragma once
#include <stddef.h>
#include <stdint.h>
#include "limine.h"

struct Color {
    uint8_t r, g, b, a;

    explicit Color();
    explicit Color(uint32_t color);
    explicit Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    [[nodiscard]] uint32_t get_bytes() const;
};

namespace Graphics {
    void initialize(const limine_framebuffer *fb);
    void swap_buffers(bool is_gui);
    void draw_pixel(uint32_t x, uint32_t y, Color color);
    void draw_rect_filled(uint32_t x, uint32_t y, size_t w, size_t h, Color color);
    uint32_t *get_terminal_buffer();
}
