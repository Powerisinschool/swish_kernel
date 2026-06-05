#include <utils/ring_buffer.h>

#include "terminal.h"

bool KeyboardBuffer::push(const uint8_t data) {
    const size_t next = (head + 1) % KEYBOARD_BUFFER_SIZE;
    if (next == tail) return false; // Buffer overflow
    buffer[head] = data;
    head = next;
    return true;
}

bool KeyboardBuffer::pop(uint8_t &data) {
    if (head == tail) return false; // Buffer empty
    data = buffer[tail];
    tail = (tail + 1) % KEYBOARD_BUFFER_SIZE;
    return true;
}
