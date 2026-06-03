#include <arch/io.h>
#include <drivers/pic.h>

#define ICW1_ICW4  0x01    // Indicates that ICW4 will be present
#define ICW1_INIT  0x10    // Initialization command
#define ICW4_8086  0x01    // 8086/88 (x86) mode

namespace PIC
{
    void remap(int offset1, int offset2)
    {
        unsigned char a1, a2;

        // Save the current interrupt masks (what is currently enabled/disabled)
        a1 = inb(PIC1_DATA);
        a2 = inb(PIC2_DATA);

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
        outb(PIC2_DATA, a2);
    }

    void send_eoi(uint8_t irq)
    {
        // If the IRQ came from the Slave PIC (IRQ 8-15), we need to send an EOI to both PICs
        if (irq >= 8)
        {
            outb(PIC2_COMMAND, PIC_EOI);
        }

        // ALWAYS send an EOI to the Master PIC, regardless of where it came from
        outb(PIC1_COMMAND, PIC_EOI);
    }

    void disable() {
        // Writing 0xFF masks (disables) all IRQs on the PIC
        outb(PIC1_DATA, 0xFF);
        outb(PIC2_DATA, 0xFF);
    }
}
