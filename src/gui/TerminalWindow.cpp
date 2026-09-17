#include <gui/TerminalWindow.h>

#include "string.h"

void TerminalWindow::inject_key(const char c) {
    if (c == '\n') {
        char command_buffer[cols + 1];
        strncpy(command_buffer, text_grid + (cursor.y * cols), cols);
        command_buffer[cols] = '\0';
        Window::inject_key('\n'); // move the cursor
        // process the command
        return;
    }
    Window::inject_key(c);
}
