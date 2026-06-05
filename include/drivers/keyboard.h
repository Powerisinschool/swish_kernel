#pragma once

namespace Keyboard {
    /**
     * @brief Initializes the keyboard driver during the system boot phase.
     *
     * This function must be called exactly once during kernel initialization.
     * It sets up internal state structures and populates the scancode-to-ASCII
     * translation tables for keyboard input processing.
     *
     * @note Must be called before enabling hardware interrupts.
     */
    void initialize();

    /**
     * @brief Hardware interrupt service routine for the keyboard.
     *
     * Invoked automatically by the Interrupt Descriptor Table (IDT) whenever
     * a physical keyboard event occurs. It reads raw scancodes from the input port,
     * processes make/break codes, and updates the internal key state tracking.
     *
     * @warning This function executes in an interrupt context and must remain
     *          fast, deterministic, and non-blocking.
     */
    void handle_interrupt();

    /**
     * @brief Disables the keyboard driver from processing keystrokes
     */
    void disable();
}
