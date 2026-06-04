#pragma once
#include <stdint.h>
#include <stddef.h>

struct KeyState {
    bool is_down;
    bool is_printable;
    char ascii_lowercase;
    char ascii_uppercase;
};

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
     * @brief Checks the current real-time state of a specific keyboard key.
     *
     * Queries the driver's internal state map to determine if a key is pressed.
     * This provides a non-blocking way for the kernel or system processes to
     * poll for continuous input, such as modifier keys or game controls.
     *
     * @param scancode The raw hardware or standardized scancode of the target key.
     * @return true If the key corresponding to the scancode is currently pressed.
     * @return false If the key is released or has not yet been registered.
     */
    bool is_key_down(uint8_t scancode);

    /**
     * @brief Reads a line of text from the keyboard input stream.
     *
     * Synchronously retrieves characters from the keyboard buffer until a
     * newline character ('\n') is encountered or the buffer limit is reached.
     * This call blocks the execution of the calling thread until input is complete.
     *
     * @param[out] buffer Pointer to the character array where input will be stored.
     * @param[in] max_len Maximum number of characters to read, including the null terminator.
     *
     * @pre Interrupts must be enabled and the keyboard driver initialized.
     */
    void getline(char *buffer, size_t max_len);
}
