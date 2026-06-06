#include <subsystems/input.h>
#include <stdint.h>
#include <utils/ring_buffer.h>

#include "string.h"
#include "terminal.h"
#include "gui/Graphics.h"

#define EXTENSION_CODE 0xE0
#define RELEASE_KEY_CODE 0x80
#define BACKSPACE_KEY_CODE 0x0E
#define ENTER_KEY_CODE 0x1C
#define CAPS_LOCK_KEY_CODE 0x3A
#define LEFT_SHIFT_KEY_CODE 0x2A
#define RIGHT_SHIFT_KEY_CODE 0x36
#define CTRL_KEY_CODE 0x1D
#define ALT_KEY_CODE 0x38
#define LEFT_ARROW_KEY_CODE 0x4B
#define RIGHT_ARROW_KEY_CODE 0x4D
#define RELEASED_KEY(keyCode) (keyCode + RELEASE_KEY_CODE)

#define KEY_UP_ESC    "\033[A"
#define KEY_DOWN_ESC  "\033[B"
#define KEY_RIGHT_ESC "\033[C"
#define KEY_LEFT_ESC  "\033[D"

#define LINE_BUFFER_SIZE 1024

// static bool is_terminal_enabled = true;

static volatile bool line_ready = false;

static KeyState keyboard_state[256] = {};
static bool is_extended = false;
static bool is_caps = false;
static bool is_shift_active = false;
static bool is_ctrl_active = false;
static bool is_alt_active = false;

static char input_buffer[LINE_BUFFER_SIZE];
static uint16_t to_input_index = 0;
static uint16_t input_len = 0;

static auto active_context = DisplayContext::TERMINAL;

// A standard US QWERTY layout map for the first 128 scancodes
static constexpr char qwerty_lower[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
    '*',  0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    // The rest default to 0 (non-printable)
};

static constexpr char qwerty_upper[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '"', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?',   0,
    '*',  0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    // The rest default to 0 (non-printable)
};

KeyboardBuffer kbd_ring_buffer;

namespace Input {
    void initialize() {
        for (int i = 0; i < 128; i++) {
            keyboard_state[i].is_down = false;
            keyboard_state[i].ascii_lowercase = qwerty_lower[i];
            if (qwerty_lower[i] >= 'a' && qwerty_lower[i] <= 'z') {
                keyboard_state[i].ascii_uppercase = static_cast<char>(qwerty_lower[i] - 32);
            } else {
                keyboard_state[i].ascii_uppercase = qwerty_upper[i];
            }
            keyboard_state[i].is_printable = (qwerty_lower[i] != 0);
        }
    }

    void process_events() {
        uint8_t scancode;

        while (kbd_ring_buffer.pop(scancode)) {
            // Handle Extension Codes
            if (scancode == EXTENSION_CODE) {
                is_extended = true;
                continue;
            }

            const bool is_release = (scancode >= RELEASE_KEY_CODE);
            uint8_t index = is_release ? (scancode - RELEASE_KEY_CODE) : scancode;

            if (is_extended) {
                index += 128;
                is_extended = false;
            }

            keyboard_state[index].is_down = !is_release;

            if (scancode == CAPS_LOCK_KEY_CODE) {
                is_caps = !is_caps;
                continue;
            }

            if (scancode == LEFT_SHIFT_KEY_CODE || scancode == RIGHT_SHIFT_KEY_CODE) {
                is_shift_active = true;
                continue;
            }

            if (scancode == CTRL_KEY_CODE) {
                is_ctrl_active = true;
                continue;
            }

            if (scancode == ALT_KEY_CODE) {
                is_alt_active = true;
                continue;
            }

            if (scancode == RELEASED_KEY(LEFT_SHIFT_KEY_CODE) || scancode == RELEASED_KEY(RIGHT_SHIFT_KEY_CODE)) {
                is_shift_active = false;
                continue;
            }

            if (scancode == RELEASED_KEY(CTRL_KEY_CODE)) {
                is_ctrl_active = false;
                continue;
            }

            if (scancode == RELEASED_KEY(ALT_KEY_CODE)) {
                is_alt_active = false;
                continue;
            }

            if (scancode == BACKSPACE_KEY_CODE) {
                if (active_context == DisplayContext::TERMINAL) {
                    if (to_input_index == 0) continue;

                    send_to_terminal('\b', false, false);
                    send_to_terminal(' ', false, false);
                    send_to_terminal('\b', false, false);
                    // input_buffer[to_input_index] = '\0';
                    to_input_index--;
                    send_to_terminal('\0', true, false, false);
                } else {
                    send_to_gui('\b');
                }
                continue;
            }

            if (scancode == ENTER_KEY_CODE) {
                if (active_context == DisplayContext::TERMINAL) {
                    cout << "\r\n";
                    input_buffer[to_input_index] = '\0';
                    line_ready = true;
                    // cout << "(input_buffer): " << input_buffer << "\r\n";
                } else {
                    send_to_gui('\n');
                }
                continue;
            }

            if (!is_release) {
                // Check for context switch shortcut (Ctrl + Alt + 1 or 0)
                if (is_ctrl_active && is_alt_active) {
                    if (index == 0x14) { // Scancode for 'T'
                        switch_to_terminal();
                        continue;
                    }
                    if (index == 0x20) { // Scancode for 'G'
                        switch_to_gui();
                        continue;
                    }
                }

                // 5. Normal Character Processing
                if (keyboard_state[index].is_printable) {
                    const char c = determine_case(index); // Helper function using your QWERTY logic
                    send_to_active_context(c);
                }
            }
        }
    }

    void get_line(char *buffer, size_t max_len) {
        while (!line_ready) {
            __asm__ __volatile__("cli");
            process_events();
            // __asm__ __volatile__("hlt"); // Block the execution thread until the line is ready
            if (!line_ready) {
                Graphics::swap_buffers(active_context == DisplayContext::GUI);
                // sti enables interrupts, and hlt immediately waits for one.
                // This specific sequence prevents the CPU from sleeping forever.
                __asm__ __volatile__("sti");
                __asm__ __volatile__("hlt");
            } else {
                // The line is ready, just re-enable interrupts and exit
                __asm__ __volatile__("sti");
            }
        }

        Graphics::swap_buffers(active_context == DisplayContext::GUI);

        size_t copy_len = to_input_index < max_len - 1 ? to_input_index : max_len - 1;

        memcpy(buffer, input_buffer, copy_len);
        buffer[copy_len] = '\0';

        memset(&input_buffer, 0, LINE_BUFFER_SIZE);
        to_input_index = 0;
        input_len = 0;
        line_ready = false;
    }

    char determine_case(const uint8_t index) {
        const bool is_letter = (keyboard_state[index].ascii_lowercase >= 'a' && keyboard_state[index].ascii_lowercase <= 'z');
        char c;
        if ((!is_letter && is_shift_active) || (is_letter && ((is_shift_active && !is_caps) || is_caps && !is_shift_active)))
            c = keyboard_state[index].ascii_uppercase;
        else
            c = keyboard_state[index].ascii_lowercase;
        return c;
    }

    void switch_to_terminal() {
        // is_terminal_enabled = true;
        if (active_context == DisplayContext::TERMINAL) return;
        // if (active_context == DisplayContext::GUI) {
        //     char s[input_len];
        //     memset(s, ' ', input_len);
        //     Graphics::draw_string(10, 10, s, Graphics::get_bg());
        // }

        active_context = DisplayContext::TERMINAL;
        // cout << "\r\n[Terminal Context Restored]\r\nuser@kernel:~$ ";
        // for (size_t i = 0; i < to_input_index; i++) {
        //     send_to_terminal(input_buffer[i], false, false, true);
        // }
    }

    void switch_to_gui() {
        // is_terminal_enabled = false;
        // cout << "\r\n";
        active_context = DisplayContext::GUI;
        // Graphics::draw_string(10, 10, "Current input:", Color::white);
        // Graphics::draw_string(10, 30, input_buffer, Color::white);
        // Graphics::swap_buffers(active_context == DisplayContext::GUI);
    }

    void send_to_terminal(const char c, const bool should_buffer, const bool should_increment, const bool should_print) {
        if (active_context != DisplayContext::TERMINAL) return;
        if (should_buffer) {
            input_buffer[to_input_index] = c;
            if (should_increment) {
                to_input_index++;
                if (to_input_index >= input_len)
                    input_len++;
            }
        }
        if (should_print) cout << c;
    }

    void send_to_gui(const char c) {
        // cout << "GUI is currently active\r\n";
        (void)c;
    }

    void send_to_active_context(const char c) {
        if (active_context == DisplayContext::TERMINAL) {
            send_to_terminal(c);
        } else if (active_context == DisplayContext::GUI) {
            send_to_gui(c);
        }
    }

    DisplayContext &get_display_context() {
        return active_context;
    }
}
