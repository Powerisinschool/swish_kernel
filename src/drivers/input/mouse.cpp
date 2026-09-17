#include <drivers/mouse.h>

#include "terminal.h"
#include "arch/x86_64/io.h"
#include "subsystems/input.h"

bool mouseDown = false;

static uint8_t mouse_cycle = 0;
static uint8_t mouse_packet[3];

namespace Mouse {
    void enable_ps2_mouse_interrupts() {
        // Step 1: Tell controller we want to read the Configuration Byte
        while (inb(0x64) & 0x02) asm("pause");
        outb(0x64, 0x20);

        while (!(inb(0x64) & 0x01)) asm("pause");
        uint8_t config = inb(0x60);

        // Step 2: Enable Bit 1 (Mouse IRQ 12) and clear Bit 5 (Mouse Clock Disable)
        config |= 0x01;   // Enable keyboard interrupt
        config |= 0x02;   // Enable mouse interrupt
        config &= ~0x10;  // Enable keyboard clock (clear bit 4)
        config &= ~0x20;  // Enable mouse clock (clear bit 5)

        // Step 3: Tell controller we want to write the Configuration Byte
        while (inb(0x64) & 0x02) asm("pause");
        outb(0x64, 0x60);

        while (inb(0x64) & 0x02) asm("pause");
        outb(0x60, config);
    }

    void initialize() {
        while (inb(0x64) & 0x01) {
            (void)inb(0x60);
        }

        while (inb(0x64) & 0x02) asm("pause");
        outb(0x64, 0xA8);

        enable_ps2_mouse_interrupts();

        while (inb(0x64) & 0x02) asm("pause");
        outb(0x64, 0xD4);

        while (inb(0x64) & 0x02) asm("pause");
        outb(0x60, 0xF4);

        while (!(inb(0x64) & 0x01)) asm("pause");
        uint8_t ack = inb(0x60);
        (void)ack;
    }

    void handle_interrupt() {
        const uint8_t status = inb(0x64);
        if (!(status & 0x01)) return;
        if (!(status & 0x20)) return;

        const uint8_t data = inb(0x60);

        if (Input::get_display_context() != DisplayContext::GUI) {
            mouse_cycle = 0;
            return;
        }

        switch (mouse_cycle) {
            case 0:
                // Bit 3 of byte 0 must be 1 in a valid PS/2 packet
                if (data & 0x08) {
                    mouse_packet[0] = data;
                    mouse_cycle++;
                }
                break;
            case 1:
                mouse_packet[1] = data;
                mouse_cycle++;
                break;
            case 2:
                mouse_packet[2] = data;
                // This is the final byte in the 3-byte sequence
                mouse_ring_buffer.push(process_mouse_packet());
                mouse_cycle = 0;
                break;
            default:
                // Something has gone wrong as we should never reach this case
                mouse_cycle = 0;
                memset(mouse_packet, 0, sizeof(mouse_packet));
                break;
        }
    }

    MouseEvent process_mouse_packet() {
        int16_t deltaX = mouse_packet[1];
        if (mouse_packet[0] & 0x10) {
            deltaX -= 256;
        }

        int16_t deltaY = mouse_packet[2];
        if (mouse_packet[0] & 0x20) {
            deltaY -= 256;
        }
        // Screen coordinates are positive for down, but PS/2 mouse gives in reverse
        deltaY = static_cast<int16_t>(-deltaY);

        return MouseEvent{deltaX, deltaY, mouse_packet[0]};
    }
}
