#include <gui/TerminalWindow.h>

#include "string.h"

TerminalWindow::TerminalWindow(size_t x, size_t y, size_t width, size_t height, const char *prompt_string) : Window(x, y, width, height, Color::white), prompt(prompt_string), prompt_len(strlen(prompt_string)) {
    print(prompt);
}

void TerminalWindow::inject_key(const char c) {
    if (c == '\n') {
        char command_buffer[cols + 1];
        strncpy(command_buffer, text_grid + (cursor.y * cols) + prompt_len, cols - prompt_len);
        command_buffer[cols - prompt_len] = '\0';
        Window::inject_key('\n'); // move the cursor
        // process the command
        if (Shell::getInstance().eval_user_input(command_buffer, this)) {
            should_close = true;
            return;
        }
        print(prompt);
        return;
    }
    if (c == '\b') {
        if (cursor.x > prompt_len) Window::inject_key('\b');
        return;
    }
    Window::inject_key(c);
}

void TerminalWindow::print(const char c) {
    Window::inject_key(c);
}

void TerminalWindow::print(const String &text) {
    for (const auto c : text) {
        Window::inject_key(c);
    }
}

TerminalWindow &TerminalWindow::operator<<(const char *str) {
    print(str);
    return *this;
}

TerminalWindow &TerminalWindow::operator<<(const String &str) {
    print(str);
    return *this;
}

TerminalWindow &TerminalWindow::operator<<(const char c) {
    print(c);
    return *this;
}

TerminalWindow &TerminalWindow::operator<<(int64_t num) {
    if (num == 0)
    {
        *this << '0';
        return *this;
    }

    if (num < 0)
    {
        *this << '-';
        num = -num;
    }

    char buffer[20]; // Large enough for a 64-bit integer
    int index = 0;

    // Extract digits one by one (this grabs them backwards)
    while (num > 0)
    {
        buffer[index++] = '0' + (num % 10);
        num /= 10;
    }

    // Print the buffer in reverse order to correct it
    while (index > 0)
    {
        index--;
        *this << buffer[index];
    }

    return *this;
}
