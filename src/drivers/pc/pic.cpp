#include <arch/x86_64/io.h>
#include <drivers/pic.h>

#define ICW1_ICW4  0x01    // Indicates that ICW4 will be present
#define ICW1_INIT  0x10    // Initialization command
#define ICW4_8086  0x01    // 8086/88 (x86) mode

namespace PIC
{
    void remap(const int offset1, const int offset2)
    {
        // Save the current interrupt masks (what is currently enabled/disabled)
        const unsigned char a1 = inb(PIC1_DATA);
        const unsigned char a2 = inb(PIC2_DATA);

        // ICW1: Start the initialization sequence in cascade mode
        outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
        io_wait();
        outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
        io_wait();

        // ICW2: Set the new offsets (Usually 0x20 for Master, 0x28 for Slave)
        outb(PIC1_DATA, offset1);
        io_wait();
        outb(PIC2_DATA, offset2);
        io_wait();

        // ICW3: Tell Master PIC that there is a Slave PIC at IRQ2 (0000 0100)
        outb(PIC1_DATA, 4);
        io_wait();
        // ICW3: Tell Slave PIC it's cascade identity (0000 0010)
        outb(PIC2_DATA, 2);
        io_wait();

        // ICW4: Set x86 mode
        outb(PIC1_DATA, ICW4_8086);
        io_wait();
        outb(PIC2_DATA, ICW4_8086);
        io_wait();

        // Restore the saved masks
        outb(PIC1_DATA, a1);
        io_wait();
        outb(PIC2_DATA, a2);
        io_wait();
    }

    void send_eoi(const uint8_t irq)
    {
        // If the IRQ came from the Slave PIC (IRQ 8-15), we need to send an EOI to both PICs
        if (irq >= 8)
        {
            outb(PIC2_COMMAND, PIC_EOI);
        }

        // ALWAYS send an EOI to the Master PIC, regardless of where it came from
        outb(PIC1_COMMAND, PIC_EOI);
    }

    void enable() {
        // Unmask IRQs 0-7 on the Master PIC
        outb(PIC1_DATA, 0xF8); // Binary: 11111000 -> Hex: 0xF8
        // Unmask IRQs 0-7 on the Slave PIC
        outb(PIC2_DATA, 0xEF); // Binary: 11101111 -> Hex: 0xEF
    }

    void disable() {
        // Writing 0xFF masks (disables) all IRQs on the PIC
        outb(PIC1_DATA, 0xFF);
        outb(PIC2_DATA, 0xFF);
    }

    void set_mask(uint8_t irq) {
        uint16_t port;

        if (irq < 8) {
            port = PIC1_DATA;
        } else {
            port = PIC2_DATA;
            irq -= 8;
        }
        const uint8_t value = inb(port) | (1 << irq);
        outb(port, value);
    }

    void clear_mask(uint8_t irq) {
        uint16_t port;

        if (irq < 8) {
            port = PIC1_DATA;
        } else {
            port = PIC2_DATA;
            irq -= 8;
        }
        const uint8_t value = inb(port) & ~(1 << irq);
        outb(port, value);
    }
}
