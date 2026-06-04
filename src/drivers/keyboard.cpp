#include <drivers/keyboard.h>
#include <arch/io.h>

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

static KeyState keyboard_state[256] = {};
static bool is_extended = false;
static bool is_caps = false;
static bool is_shift_active = false;

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
            cout << "\b \b";
            return;
        }

        if (scancode == ENTER_KEY_CODE) {
            cout << "\r\n";
            return;
        }

        if (scancode == LEFT_ARROW_KEY_CODE) {
            cout << KEY_LEFT_ESC;
            return;
        }

        if (scancode == RIGHT_ARROW_KEY_CODE) {
            cout << KEY_RIGHT_ESC;
            return;
        }

        const bool is_num = scancode > 0x01 && scancode < 0x0C;
        const bool should_capitalize = (is_shift_active) ? !is_caps : is_caps;

        if (!is_release && !is_extended && keyboard_state[index].is_printable) {
            if ((is_num && is_shift_active) || (!is_num && should_capitalize))
                cout << keyboard_state[index].ascii_uppercase;
            else
                cout << keyboard_state[index].ascii_lowercase;
        }
    }

    bool is_key_down(const uint8_t scancode) {
        return keyboard_state[scancode].is_down;
    }
}
