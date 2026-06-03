#pragma once
#include <stdint.h>

#define KERNEL_PACKED [[gnu::packed]]

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

    // --- Type and Attribute Bitfields ---
    uint8_t  gate_type : 4; ///< Gate type (Bits 0-3. e.g., 0xE for 64-bit Interrupt, 0xF for Trap).
    uint8_t  storage   : 1; ///< Storage Segment (Bit 4. Must be 0 for system gates).
    uint8_t  dpl       : 2; ///< Descriptor Privilege Level (Bits 5-6. 0 = Kernel, 3 = User space).
    uint8_t  present   : 1; ///< Present flag (Bit 7. Must be 1 for a valid entry).

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
