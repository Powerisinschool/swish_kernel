#pragma once
#include <stdint.h>
#include <stddef.h>

constexpr size_t KEYBOARD_BUFFER_SIZE = 256;
constexpr size_t MOUSE_BUFFER_SIZE = 256;

class KeyboardBuffer {
public:
    bool push(uint8_t data);
    bool pop(uint8_t &data);
private:
    uint8_t buffer[KEYBOARD_BUFFER_SIZE]{};
    volatile size_t head = 0; // Where the ISR writes to
    volatile size_t tail = 0; // Where the Router reads from
};

struct MouseEvent {
    int64_t deltaX;
    int64_t deltaY;
    uint8_t buttons;
};

class MouseBuffer {
public:
    bool push(const MouseEvent &event);
    bool pop(MouseEvent &event);
private:
    MouseEvent buffer[MOUSE_BUFFER_SIZE]{};
    volatile size_t head = 0; // Where the ISR writes to
    volatile size_t tail = 0; // Where the Router reads from
};

extern KeyboardBuffer kbd_ring_buffer;
extern MouseBuffer mouse_ring_buffer;
