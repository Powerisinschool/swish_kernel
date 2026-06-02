#ifndef KERNEL_GRAPHICS_HPP
#define KERNEL_GRAPHICS_HPP

#include <stdint.h>

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    uint32_t to_uint32() const
    {
        return (r << 16) | (g << 8) | b;
    }
};

void draw_pixel(struct limine_framebuffer *fb, uint64_t x, uint64_t y, Color &color);

#endif // KERNEL_GRAPHICS_HPP