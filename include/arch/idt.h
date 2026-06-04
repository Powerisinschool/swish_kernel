#pragma once
#include <stdint.h>
#include <compiler.h>

/**
 * @brief Configuration flags for an IDT entry attributes byte.
 */
struct KERNEL_PACKED idt_flags {
    uint8_t  gate_type : 4; ///< Gate type (Bits 0-3. e.g., 0xE for 64-bit Interrupt, 0xF for Trap).
    uint8_t  storage   : 1; ///< Storage Segment (Bit 4. Must be 0 for system gates).
    uint8_t  dpl       : 2; ///< Descriptor Privilege Level (Bits 5-6. 0 = Kernel, 3 = User space).
    uint8_t  present   : 1; ///< Present flag (Bit 7. Must be 1 for a valid entry).

    // Default constructor (required if you define custom ones)
    idt_flags() = default;

    idt_flags(uint8_t gate_type, uint8_t dpl, bool present);

    explicit idt_flags(uint8_t val);

    static idt_flags kernel_interrupt();

    static idt_flags user_trap();

    explicit operator uint8_t() const;

    idt_flags &operator=(const uint8_t &value);
};

/**
 * @brief Represents a 16-byte x86_64 Interrupt Descriptor Table (IDT) entry.
 * @details This structure maps exactly to the hardware format required by the CPU in
 *          64-bit Long Mode, eliminating the need for manual bitwise operations.
 * @see [OSDev Wiki - Interrupt Descriptor Table](https://osdev.org)
 */
struct KERNEL_PACKED idt_entry {
    uint16_t isr_low;       ///< Lower 16 bits of the Interrupt Service Routine (ISR) address.
    uint16_t kernel_cs;     ///< Global Descriptor Table (GDT) code segment selector.

    // --- Interrupt Stack Table (IST) Split ---
    uint8_t  ist       : 3; ///< Interrupt Stack Table offset (Bits 0-2).
    uint8_t  reserved0 : 5; ///< Reserved bits, must be zero (Bits 3-7).

    idt_flags attributes;   ///< Type and Attribute Bitfields
    uint16_t isr_mid;       ///< Middle 16 bits of the ISR address.
    uint32_t isr_high;      ///< Upper 32 bits of the ISR address.
    uint32_t reserved1;     ///< Reserved block by architecture, must be zero.
};

/**
 * @brief Pointer structure loaded directly into the CPU's IDTR register.
 * @details This structure defines the size and linear memory location of the IDT.
 *          It is designed to be passed by reference to the `lidt` assembly instruction.
 *
 * @see [OSDev Wiki - IDT Pointer Structure](https://wiki.osdev.org/Interrupt_Descriptor_Table)
 */
struct KERNEL_PACKED idt_ptr {
    /**
     * @brief The size of the IDT in bytes minus 1.
     * @note For a maximum table of 256 entries, this value is `(sizeof(idt_entry) * 256) - 1`.
     */
    uint16_t limit;

    /**
     * @brief The linear base virtual address where the IDT array starts.
     */
    uint64_t base;
};

namespace IDT {
    /**
     * @brief Configures a single 16-byte gate entry in the 64-bit Interrupt Descriptor Table (IDT).
     *
     * Populates the specified IDT vector slot with a 64-bit interrupt service routine
     * address and configuration flags. This function safely splits the 64-bit pointer
     * across the low, mid, and high fields of the x86_64 IDT gate structure.
     *
     * @param vector The target interrupt vector index (0-255).
     * @param isr 64-bit pointer to the Interrupt Service Routine handler function.
     * @param flags Configuration bitmask defining gate type, DPL, present bit, and optional IST index.
     *
     * @note In x86_64, ensure the target ISR handles the error code pushed by the CPU for specific exceptions.
     */
    void set_descriptor(uint8_t vector, void *isr, idt_flags flags);

    /**
     * @brief Loads the 64-bit Interrupt Descriptor Table into the CPU's IDTR register.
     *
     * Constructs a 10-byte IDTR register structure containing the 16-bit table limit
     * and the 64-bit base virtual address of the IDT. It then executes the `lidt`
     * assembly instruction to activate the table on the current CPU core.
     *
     * @pre All critical hardware exceptions and PIC/APIC IRQ descriptors must be set.
     * @warning Any interrupt firing before this call finishes will result in a CPU triple fault.
     */
    void initialize();
}
