#include <drivers/keyboard.h>
#include <arch/x86_64/io.h>
#include <string.h>

#include <utils/ring_buffer.h>
#include "terminal.h"

static bool is_enabled = true;

namespace Keyboard {
    void handle_interrupt() { // ISR
        if (!is_enabled) {
            return;
        }

        // Clear the buffer and allow future interrupts
        const uint8_t scancode = inb(0x60);

        kbd_ring_buffer.push(scancode);
        // return;
        //
        // if (scancode == EXTENSION_CODE) {
        //     is_extended = true;
        //     return;
        // }
        //
        // const bool is_release = (scancode >= RELEASE_KEY_CODE);
        //
        // // if (!is_release) // Debug which key was pressed
        // //     cout << (int64_t)scancode << " ";
        //
        // uint8_t index = is_release ? (scancode - RELEASE_KEY_CODE) : scancode;
        //
        // if (is_extended) {
        //     index += 128;
        //     is_extended = false;
        // }
        //
        // keyboard_state[index].is_down = !is_release;
        //
        // if (scancode == CAPS_LOCK_KEY_CODE) {
        //     is_caps = !is_caps;
        //     return;
        // }
        //
        // if (scancode == LEFT_SHIFT_KEY_CODE || scancode == RIGHT_SHIFT_KEY_CODE) {
        //     is_shift_active = true;
        //     return;
        // }
        //
        // if (scancode == RELEASED_KEY(LEFT_SHIFT_KEY_CODE) || scancode == RELEASED_KEY(RIGHT_SHIFT_KEY_CODE)) {
        //     is_shift_active = false;
        //     return;
        // }
        //
        // if (scancode == BACKSPACE_KEY_CODE) {
        //     if (to_input_index == 0) return;
        //
        //     input_buffer[to_input_index] = '\0';
        //     to_input_index--;
        //     cout << "\b \b";
        //     return;
        // }
        //
        // if (scancode == ENTER_KEY_CODE) {
        //     cout << "\r\n";
        //     input_buffer[to_input_index] = '\0';
        //     line_ready = true;
        //     // cout << "(input_buffer): " << input_buffer << "\r\n";
        //     return;
        // }
        //
        // // if (scancode == LEFT_ARROW_KEY_CODE) {
        // //     cout << KEY_LEFT_ESC;
        // //     if (to_input_index > 0)
        // //         to_input_index--;
        // //     return;
        // // }
        // //
        // // if (scancode == RIGHT_ARROW_KEY_CODE) {
        // //     cout << KEY_RIGHT_ESC;
        // //     if (to_input_index <= input_len)
        // //         to_input_index++;
        // //     return;
        // // }
        //
        // // const bool is_num = scancode > 0x01 && scancode < 0x0C;
        // const bool should_capitalize = (is_shift_active) ? !is_caps : is_caps;
        //
        // if (!is_release && !is_extended && keyboard_state[index].is_printable) {
        //     if (to_input_index >= LINE_BUFFER_SIZE - 1) {
        //         cout << "\r\nFailed to input (buffer overflow)\r\n" << "\a";
        //         return;
        //     };
        //     const bool is_letter = (keyboard_state[index].ascii_lowercase >= 'a' && keyboard_state[index].ascii_lowercase <= 'z');
        //     char c;
        //     if ((!is_letter && is_shift_active) || (is_letter && should_capitalize))
        //         c = keyboard_state[index].ascii_uppercase;
        //     else
        //         c = keyboard_state[index].ascii_lowercase;
        //     input_buffer[to_input_index] = c;
        //     cout << c;
        //     to_input_index++;
        //     if (to_input_index >= input_len)
        //         input_len++;
        // }
    }

    // bool is_key_down(const uint8_t scancode) {
    //     return keyboard_state[scancode].is_down;
    // }
    //
    // void getline(char *buffer, size_t max_len) {
    //     if (!is_enabled) {
    //         buffer[0] = '\0';
    //         return;
    //     }
    //
    //     while (!line_ready && is_enabled) {
    //         __asm__ __volatile__("hlt"); // Block the execution thread until the line is ready
    //     }
    //
    //     size_t copy_len = to_input_index < max_len - 1 ? to_input_index : max_len - 1;
    //
    //     memcpy(buffer, input_buffer, copy_len);
    //     buffer[copy_len] = '\0';
    //
    //     memset(&input_buffer, 0, LINE_BUFFER_SIZE);
    //     to_input_index = 0;
    //     input_len = 0;
    //     line_ready = false;
    // }

    void disable() {
        is_enabled = false;
    }
}
