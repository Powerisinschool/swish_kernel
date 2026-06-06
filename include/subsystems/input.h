#pragma once
#include <stddef.h>
#include <stdint.h>

enum class DisplayContext {
    TERMINAL,
    GUI
};

struct KeyState {
    bool is_down;
    bool is_printable;
    char ascii_lowercase;
    char ascii_uppercase;
};

namespace Input {
    /**
     * @brief Initializes the input router during the system boot phase.
     *
     * This function must be called exactly once during kernel initialization.
     * It sets up internal state structures and populates the scancode-to-ASCII
     * translation tables for keyboard input processing.
     *
     * @note Must be called before enabling hardware interrupts.
     */
    void initialize();

    /**
     * @brief Processes keyboard scancodes from the input queue
     */
    void process_events();

    /**
     * @deprecated Prefer non-blocking
     * @brief Reads a line of text from the keyboard input stream.
     *
     * Synchronously retrieves characters from the keyboard buffer until a
     * newline character ('\n') is encountered or the buffer limit is reached.
     * This call blocks the execution of the calling thread until input is complete.
     *
     * @param[out] buffer Pointer to the character array where input will be stored.
     * @param[in] max_len Maximum number of characters to read, including the null terminator.
     *
     * @pre Interrupts must be enabled and the input router initialized.
     */
    void get_line(char *buffer, size_t max_len);
    bool is_line_ready();
    void fetch_line(char *buffer, size_t max_len);
    bool is_terminal_dirty();
    void flush_terminal_updates();
    char determine_case(uint8_t index);
    void switch_to_terminal();
    void switch_to_gui();
    void send_to_terminal(char c, bool should_buffer = true, bool should_increment = true, bool should_print = true);
    void send_to_gui(char c);
    void send_to_active_context(char c);
    DisplayContext &get_display_context();
}
