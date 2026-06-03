#pragma once
#include <stdint.h>

#define PIC1_COMMAND 0x20 // Master PIC command port
#define PIC1_DATA    0x21 // Master PIC data port
#define PIC2_COMMAND 0xA0 // Slave PIC command port
#define PIC2_DATA    0xA1 // Slave PIC data port

#define PIC_EOI      0x20 // End of Interrupt command code

namespace PIC {
    // Initializes and remaps the PICs to a new base offset
    void remap(int offset1, int offset2);

    // Sends the End of Interrupt signal to the hardware
    void send_eoi(uint8_t irq);

    // Enables the legacy PIC (Limine disables it by default)
    void enable();

    // Completely disables the legacy PIC (useful when moving to APIC later)
    void disable();
}
