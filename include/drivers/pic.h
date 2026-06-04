#pragma once
#include <stdint.h>

#define PIC1_COMMAND 0x20 // Master PIC command port
#define PIC1_DATA    0x21 // Master PIC data port
#define PIC2_COMMAND 0xA0 // Slave PIC command port
#define PIC2_DATA    0xA1 // Slave PIC data port

#define PIC_EOI      0x20 // End of Interrupt command code

namespace PIC {
    /**
     * @brief Remaps the Master and Slave Programmable Interrupt Controllers (PIC).
     *
     * Changes the vector offsets of the PICs to prevent hardware interrupts from
     * overlapping with CPU exceptions (Vectors 0x00-0x1F). Master PIC IRQs (0-7)
     * are mapped to offset1, and Slave PIC IRQs (8-15) are mapped to offset2.
     *
     * @param offset1 The new interrupt vector base for the Master PIC (typically 0x20).
     * @param offset2 The new interrupt vector base for the Slave PIC (typically 0x28).
     */
    void remap(int offset1, int offset2);

    /**
     * @brief Sends an End of Interrupt (EOI) command to the PIC.
     *
     * Informs the hardware that the current interrupt service routine has finished.
     * If the interrupt originated from the Slave PIC (IRQ >= 8), an EOI command
     * is issued to both the Slave and Master controllers.
     *
     * @param irq The hardware IRQ number (0-15) that is currently being serviced.
     *
     * @warning Missing an EOI will cause the PIC to block all future interrupts.
     */
    void send_eoi(uint8_t irq);

    /**
     * @brief Enables the legacy 8259 PIC.
     *
     * Unmasks the PIC interrupt lines and restores hardware interrupt delivery.
     * This is explicitly required when using bootloaders like Limine, which
     * mask and disable the legacy PIC by default before handing control over.
     *
     * @pre PIC must be remapped via remap() to avoid exception conflicts.
     */
    void enable();

    /**
     * @brief Completely disables and masks the legacy 8259 PIC.
     *
     * Disables hardware interrupt lines by setting all bits in the Interrupt
     * Mask Registers (IMR). This ensures legacy hardware interrupts do not fire,
     * which is a required step before transitioning the OS to the APIC.
     */
    void disable();
}
