#pragma once
#include <stdint.h>

#include "utils/ring_buffer.h"

namespace Mouse {
    void enable_ps2_mouse_interrupts();
    void initialize();
    void handle_interrupt();

    MouseEvent process_mouse_packet();
}
