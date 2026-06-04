#include <drivers/keyboard.h>
#include <arch/io.h>
#include <string.h>

#include "terminal.h"

#define EXTENSION_CODE 0xE0
#define RELEASE_KEY_CODE 0x80
#define BACKSPACE_KEY_CODE 0x0E
#define ENTER_KEY_CODE 0x1C
#define CAPS_LOCK_KEY_CODE 0x3A
#define LEFT_SHIFT_KEY_CODE 0x2A
#define RIGHT_SHIFT_KEY_CODE 0x36
#define LEFT_ARROW_KEY_CODE 0x4B
#define RIGHT_ARROW_KEY_CODE 0x4D
#define RELEASED_KEY(keyCode) (keyCode + RELEASE_KEY_CODE)

#define KEY_UP_ESC    "\033[A"
#define KEY_DOWN_ESC  "\033[B"
#define KEY_RIGHT_ESC "\033[C"
#define KEY_LEFT_ESC  "\033[D"

#define LINE_BUFFER_SIZE 1024

static volatile bool line_ready = false;

static KeyState keyboard_state[256] = {};
static bool is_extended = false;
static bool is_caps = false;
static bool is_shift_active = false;

static char input_buffer[LINE_BUFFER_SIZE];
static uint16_t to_input_index = 0;
static uint16_t input_len = 0;

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

namespace Keyboard {
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

    void handle_interrupt() {
        const uint8_t scancode = inb(0x60); // Clear the buffer and allow future interrupts

        if (scancode == EXTENSION_CODE) {
            is_extended = true;
            return;
        }

        const bool is_release = (scancode >= RELEASE_KEY_CODE);

        // if (!is_release) // Debug which key was pressed
        //     cout << (int64_t)scancode << " ";

        uint8_t index = is_release ? (scancode - RELEASE_KEY_CODE) : scancode;

        if (is_extended) {
            index += 128;
            is_extended = false;
        }

        keyboard_state[index].is_down = !is_release;

        if (scancode == CAPS_LOCK_KEY_CODE) {
            is_caps = !is_caps;
            return;
        }

        if (scancode == LEFT_SHIFT_KEY_CODE || scancode == RIGHT_SHIFT_KEY_CODE) {
            is_shift_active = true;
            return;
        }

        if (scancode == RELEASED_KEY(LEFT_SHIFT_KEY_CODE) || scancode == RELEASED_KEY(RIGHT_SHIFT_KEY_CODE)) {
            is_shift_active = false;
            return;
        }

        if (scancode == BACKSPACE_KEY_CODE) {
            if (to_input_index == 0) return;

            input_buffer[to_input_index] = '\0';
            to_input_index--;
            cout << "\b \b";
            return;
        }

        if (scancode == ENTER_KEY_CODE) {
            cout << "\r\n";
            input_buffer[to_input_index] = '\0';
            line_ready = true;
            // cout << "(input_buffer): " << input_buffer << "\r\n";
            return;
        }

        // if (scancode == LEFT_ARROW_KEY_CODE) {
        //     cout << KEY_LEFT_ESC;
        //     if (to_input_index > 0)
        //         to_input_index--;
        //     return;
        // }
        //
        // if (scancode == RIGHT_ARROW_KEY_CODE) {
        //     cout << KEY_RIGHT_ESC;
        //     if (to_input_index <= input_len)
        //         to_input_index++;
        //     return;
        // }

        // const bool is_num = scancode > 0x01 && scancode < 0x0C;
        const bool should_capitalize = (is_shift_active) ? !is_caps : is_caps;

        if (!is_release && !is_extended && keyboard_state[index].is_printable) {
            if (to_input_index >= LINE_BUFFER_SIZE - 1) {
                cout << "\r\nFailed to input (buffer overflow)\r\n" << "\a";
                return;
            };
            const bool is_letter = (keyboard_state[index].ascii_lowercase >= 'a' && keyboard_state[index].ascii_lowercase <= 'z');
            char c;
            if ((!is_letter && is_shift_active) || (is_letter && should_capitalize))
                c = keyboard_state[index].ascii_uppercase;
            else
                c = keyboard_state[index].ascii_lowercase;
            input_buffer[to_input_index] = c;
            cout << c;
            to_input_index++;
            if (to_input_index >= input_len)
                input_len++;
        }
    }

    bool is_key_down(const uint8_t scancode) {
        return keyboard_state[scancode].is_down;
    }

    void getline(char *buffer, size_t max_len) {
        while (!line_ready) {
            __asm__ __volatile__("hlt"); // Block the execution thread until the line is ready
        }

        size_t copy_len = to_input_index < max_len - 1 ? to_input_index : max_len - 1;

        memcpy(buffer, input_buffer, copy_len);
        buffer[copy_len] = '\0';

        memset(&input_buffer, 0, LINE_BUFFER_SIZE);
        to_input_index = 0;
        input_len = 0;
        line_ready = false;
    }
}
