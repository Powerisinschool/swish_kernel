#include <stdint.h>
#include <limine.h>
#include <compiler.h>
#include <terminal.h>

#include "arch/idt.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "gui/Graphics.h"
#include "kernel/String.hpp"
#include "kernel/Shell.h"
#include "subsystems/input.h"

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

// static uint8_t initial_heap_space[1024 * 1024];
void initialize_physical_memory();

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

    initialize_physical_memory();

    Graphics::initialize(fb);
    Graphics::draw_rect_filled(fb->width/4, fb->height/4, fb->width/2, fb->height/2, Color{0xFF0000FF});

    struct flanterm_context *ft_ctx = flanterm_fb_init(
        nullptr,
        nullptr,
        Graphics::get_terminal_buffer(),
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

    // init_heap(reinterpret_cast<uintptr_t>(&initial_heap_space), sizeof(initial_heap_space));

    cout << "Hello" << ' ' << "World" << " from Flanterm!\r\n";
    cout << "Framebuffer resolution: " << static_cast<int64_t>(fb->width) << "x" << static_cast<int64_t>(fb->height) << "\r\n";

    Input::initialize();
    PIC::remap(0x20, 0x28);
    PIC::enable();
    IDT::initialize();

    // __asm__ volatile ("int $12");

    // cout << "Welcome to the custom OS!\r\n";
    // char shell_buf[256];
    //
    // while (true) {
    //     cout << "user@kernel:~$ ";
    //     Keyboard::getline(shell_buf, 256);
    //     cout << "You typed: " << shell_buf << "\r\n";
    // }

    char *line;
    Shell &shell = Shell::getInstance();

    shell.homeEnv = "/";
    shell.pathEnv = "/bin";
    shell.HISTORY_FILE = "/.history";

    constexpr size_t MAX_LINE_LEN = 1024;
    char buffer[MAX_LINE_LEN];

    k_print("Kernel booted. Memory managed by Limine.\r\n\n");

    while (true) {
        cout << "user@kernel:~$ ";
        Input::get_line(buffer, MAX_LINE_LEN);
        String input(buffer);
        // if (input.empty())
        //     continue;
        add_history(input);

        if (shell.eval_user_input(input))
            break;
    }

    Keyboard::disable();
    cout << "\r\n[Process completed]\r\n";

    Graphics::swap_buffers(Input::get_display_context() == DisplayContext::GUI);

    while (true)
    {
        __asm__ volatile("hlt");
    }
}
