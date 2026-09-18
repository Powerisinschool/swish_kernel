#include <stdint.h>
#include <limine.h>
#include <compiler.h>
#include <terminal.h>

#include "arch/hal/cpu.h"
#include "arch/x86_64/idt.h"
#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include "drivers/pic.h"
#include "gui/Compositor.h"
#include "gui/Graphics.h"
#include "gui/TerminalWindow.h"
#include "gui/Window.h"
#include "kernel/fs.hpp"
#include "kernel/ramfs.hpp"
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
            arch_halt_cpu();
        }
    }
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    initialize_physical_memory();

    auto rootSurface = Graphics::initialize(fb);
    Graphics::set_bg(&rootSurface, Color{0, 100, 150, 255});

    // Create 3 overlapping windows
    // window1 will be at the bottom (Z-index 0)
    auto *window1 = new  Window(fb->width / 8, fb->height / 8, 400, 300, Color::light_gray);
    window1->set_title("Window 1");
    // window2 will be in the middle (Z-index 1)
    auto *window2 = new Window(fb->width / 4, fb->height / 4, 350, 250, Color::green);
    window2->set_title("Window 2");
    // window3 will be on top (Z-index 2)
    auto *window3 = new Window(fb->width / 2, fb->height / 3, 300, 200, Color::yellow);
    window3->set_title("Window 3");
    // window4 will be on top (Z-index 3)
    const size_t win4X_cell = 8;
    const size_t win4y_cell = 7;
    size_t win4X = fb->width / win4X_cell;
    size_t win4y = fb->height / win4y_cell;
    auto *window4 = new TerminalWindow(win4X, win4y, win4X * (win4X_cell - 2), win4y * (win4y_cell - 2), "window4> ");
    window4->set_title("Terminal");

    Compositor compositor(&rootSurface);
    // Add them to the compositor
    compositor.add_window(window1);
    compositor.add_window(window2);
    compositor.add_window(window3);
    compositor.add_window(window4);
    compositor.render();

    flanterm_context *ft_ctx = flanterm_fb_init(
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
        0,
        true);

    cout.initialize(ft_ctx);

    // init_heap(reinterpret_cast<uintptr_t>(&initial_heap_space), sizeof(initial_heap_space));

    cout << "Hello" << ' ' << "World" << " from Flanterm!\r\n";
    cout << "Framebuffer resolution: " << static_cast<int64_t>(fb->width) << "x" << static_cast<int64_t>(fb->height) << "\r\n";

    Input::initialize();
    Mouse::initialize();
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

    Shell &shell = Shell::getInstance();

    shell.homeEnv = "/";
    shell.pathEnv = "/bin";
    shell.HISTORY_FILE = "/.history";

    constexpr size_t MAX_LINE_LEN = 1024;

    k_print("Kernel booted. Memory managed by Limine.\r\n\n");

    cout << "user@kernel:~$ ";
    // while (true) {
    //     Input::get_line(buffer, MAX_LINE_LEN);
    //     String input(buffer);
    //     // if (input.empty())
    //     //     continue;
    //     add_history(input);
    //
    //     if (shell.eval_user_input(input))
    //         break;
    // }
    //

    Input::set_compositor(&compositor);

    fs_root = new RamFSDirectory();
    fs_root->set_flags(FSNodeFlags::DIRECTORY);
    vfs_set_name(fs_root, "/");

    auto test_file = new RamFSFile();
    test_file->set_flags(FSNodeFlags::FILE);
    vfs_set_name(test_file, "test.txt");
    String data = "Hello VFS!";
    vfs_write(test_file, 0, data.len(), data.c_str());

    vfs_add_child(fs_root, test_file);

    while (true) {
        __asm__ volatile("cli");
        Input::process_events();
        // Mouse::process_events();
        __asm__ volatile("sti");

        if (Input::get_display_context() == DisplayContext::TERMINAL) {
            if (Input::is_line_ready()) {
                char buffer[MAX_LINE_LEN];
                Input::fetch_line(buffer, MAX_LINE_LEN);
                if (shell.eval_user_input(String(buffer)))
                    break;
                cout << "user@kernel:~$ ";

                Input::flush_terminal_updates();
            }
            if (Input::is_terminal_dirty()) {
                Input::flush_terminal_updates();
            }
        } else if (Input::get_display_context() == DisplayContext::GUI) {
            compositor.render();
        }

        // arch_halt_cpu();
        arch_halt_cpu();
    }

    Keyboard::disable();
    cout << "\r\n[Process completed]\r\n";

    Graphics::swap_buffers(Input::get_display_context() == DisplayContext::GUI);

    while (true)
    {
        arch_halt_cpu();
    }
}
