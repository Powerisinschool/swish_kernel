#pragma once
#include <stdint.h>

struct KeyState {
    bool is_down;
    bool is_printable;
    char ascii_lowercase;
    char ascii_uppercase;
};

namespace Keyboard {
    void initialize(); ///< Called once during boot to populate the ASCII maps
    void handle_interrupt(); ///< Called by the IDT every time a key is pressed or released
    bool is_key_down(uint8_t scancode); ///< Allows the rest of the OS to check if a specific key is held down
}
