#pragma once
#include <stdint.h>

struct Color
{
    uint8_t r : 2;
    uint8_t g : 2;
    uint8_t b : 2;
    uint8_t a : 2; // drop for now

    [[nodiscard]] uint32_t to_uint32() const
    {
        return (r << 16) | (g << 8) | b;
    }
};

void draw_pixel(struct limine_framebuffer *fb, uint64_t x, uint64_t y, Color &color);
