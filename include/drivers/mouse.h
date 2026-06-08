#pragma once
#include <stdint.h>

extern uint64_t mouse_x;
extern uint64_t mouse_y;

namespace Mouse {
    void enable_ps2_mouse_interrupts();
    void initialize();
    void handle_interrupt();
    void process_mouse_packet();
}
