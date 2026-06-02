#include <stdint.h>
#include <limine.h>
#include <kernel/graphics.hpp>

__attribute__((used, section(".limine_requests"))) static volatile uint64_t limine_base_revision[3] = {0xf759927964952454, 0x2c10aa2431d4d9d7, 0x0};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = nullptr};

static const uint8_t letter_a[8][8] = {
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 0, 1, 0},
};

void draw_letter_a(struct limine_framebuffer *fb, uint64_t x, uint64_t y, Color &color)
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            if (letter_a[row][col])
            {
                draw_pixel(fb, x + col, y + row, color);
            }
        }
    }
}

extern "C" void _start()
{
    if (framebuffer_request.response == nullptr || framebuffer_request.response->framebuffer_count < 1)
    {
        // Halting immediately prevents a crash if the bootloader failed
        while (1)
        {
            __asm__ volatile("hlt");
        }
    }
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    Color white = {255, 255, 255};
    draw_letter_a(fb, 100, 100, white);

    while (1)
    {
        __asm__ volatile("hlt");
    }
}

void draw_pixel(struct limine_framebuffer *fb, uint64_t x, uint64_t y, Color &color)
{
    uint32_t *fb_ptr = reinterpret_cast<uint32_t *>(fb->address);
    uint64_t pixel_offset = (y * (fb->pitch / 4)) + x;
    fb_ptr[pixel_offset] = color.to_uint32();
}
