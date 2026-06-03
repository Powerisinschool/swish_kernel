#include <stdint.h>
#include <limine.h>
#include <compiler.h>
#include <terminal.h>

#include "arch/idt.h"
#include "drivers/pic.h"

extern "C"
{
#include <flanterm.h>
#include <flanterm_backends/fb.h>
}

KERNEL_REQUEST static volatile uint64_t limine_base_revision[3] = {0xf759927964952454, 0x2c10aa2431d4d9d7, 0x0};

KERNEL_REQUEST static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = nullptr};

extern "C" [[noreturn]] void _start()
{
    if (framebuffer_request.response == nullptr || framebuffer_request.response->framebuffer_count < 1)
    {
        // Halting immediately prevents a crash if the bootloader failed
        while (true)
        {
            __asm__ volatile("hlt");
        }
    }
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    struct flanterm_context *ft_ctx = flanterm_fb_init(
        nullptr,
        nullptr,
        static_cast<uint32_t *>(fb->address),
        fb->width,
        fb->height,
        fb->pitch,
        fb->red_mask_size,
        fb->red_mask_shift,
        fb->green_mask_size,
        fb->green_mask_shift,
        fb->blue_mask_size,
        fb->blue_mask_shift,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0, 0, 1, 0, 0, 0,
        0);

    cout.initialize(ft_ctx);
    cout << "Hello" << ' ' << "World" << " from Flanterm!\r\n";
    cout << "Framebuffer resolution: " << static_cast<int64_t>(fb->width) << "x" << static_cast<int64_t>(fb->height) << "\r\n";

    PIC::remap(0x20, 0x28);

    PIC::enable();
    // __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x01), "Nd"((uint16_t)0x21));

    IDT::initialize();
    // __asm__ volatile ("int $12");

    while (true)
    {
        __asm__ volatile("hlt");
    }
}
