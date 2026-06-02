#include <stdint.h>
#include <limine.h>
#include <string.h>
// #include <kernel/graphics.hpp>

extern "C"
{
#include <flanterm.h>
#include <flanterm_backends/fb.h>
}

__attribute__((used, section(".limine_requests"))) static volatile uint64_t limine_base_revision[3] = {0xf759927964952454, 0x2c10aa2431d4d9d7, 0x0};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = nullptr};

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

    struct flanterm_context *ft_ctx = flanterm_fb_init(
        NULL,
        NULL,
        reinterpret_cast<uint32_t *>(fb->address),
        fb->width,
        fb->height,
        fb->pitch,
        fb->red_mask_size,
        fb->red_mask_shift,
        fb->green_mask_size,
        fb->green_mask_shift,
        fb->blue_mask_size,
        fb->blue_mask_shift,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0, 0, 1, 0, 0, 0,
        0);

    // Write text to the screen!
    const char *msg = "Hello World from Flanterm!\r\n";
    flanterm_write(ft_ctx, msg, strlen(msg));
    // Color white = {255, 255, 255};
    // draw_letter_a(fb, 100, 100, white);

    while (1)
    {
        __asm__ volatile("hlt");
    }
}

// void draw_pixel(struct limine_framebuffer *fb, uint64_t x, uint64_t y, Color &color)
// {
//     uint32_t *fb_ptr = reinterpret_cast<uint32_t *>(fb->address);
//     uint64_t pixel_offset = (y * (fb->pitch / 4)) + x;
//     fb_ptr[pixel_offset] = color.to_uint32();
// }
