#pragma once
#include <stddef.h>
#include <stdint.h>
#include "limine.h"

struct Color {
    uint8_t r, g, b, a;

    explicit Color();
    explicit Color(uint32_t color);
    explicit Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    static Color white;
    static Color black;

    [[nodiscard]] Color withAlpha(uint8_t alpha) const;

    [[nodiscard]] uint32_t get_bytes() const;
};

struct Surface {
    uint32_t *buffer;
    size_t width, height, pitch;
    Color background;

    Surface(uint32_t *buffer, size_t width, size_t height, size_t pitch, Color background);
};

namespace Graphics {
    Surface initialize(const limine_framebuffer *fb);
    void swap_buffers(bool is_gui);
    // Drawing functions
    void set_bg(Surface *dest, Color color);
    void draw_bg(Surface *dest);
    void draw_pixel(Surface *dest, uint32_t x, uint32_t y, Color color);
    void draw_rect_filled(Surface *dest, uint32_t x, uint32_t y, size_t w, size_t h, Color color);
    void draw_char(Surface *dest, uint32_t x, uint32_t y, char c, Color color);
    void draw_string(Surface *dest, uint32_t x, uint32_t y, const char *str, Color color);
    uint32_t *get_buffer();
    uint32_t *get_buffer_address(uint32_t x, uint32_t y);
    uint32_t *get_terminal_buffer();
}
