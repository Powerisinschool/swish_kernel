#include <kernel/Shell.h>

#include "terminal.h"
#include "subsystems/input.h"

bool is_shell_builtin(const String& cmd) {
    if (cmd == "help" || cmd == "clear" || cmd == "echo" || cmd == "display" || cmd == "exit") {
        return true;
    }
    return false;
}

// Forward declaration of the built-in processor
bool Shell::process_builtin(const String& cmd, String* args, int argCount, OutputStream &output) {
    if (cmd == "exit") {
        return true;
    }
    if (cmd == "display") {
        Input::switch_to_gui();
        return false;
    }
    if (cmd == "echo") {
        for (int i = 1; i < argCount - 1; i++) {
            output << args[i] << " ";
        }
        output << args[argCount - 1] << "\r\n";
        return false;
    }
    if (cmd == "help") {
        display_help(output);
        return false;
    }
    output << "[Shell built-in command `" << cmd << "`]\r\n";
    if (argCount > 1) {
        output << "Args:\r\n";
    }
    for (int i = 1; i < argCount; i++) {
        output << "    - " << args[i] << "\r\n";
    }
    return false;
}

Shell &Shell::getInstance() {
    static Shell instance;
    return instance;
}

bool Shell::eval_user_input(const String &rawInput, OutputStream *out) {
    if (rawInput.empty()) {
        return false;
    }

    constexpr int MAX_ARGS = 16;
    String args[MAX_ARGS];
    int argCount = 0;

    const char* str = rawInput.c_str();
    char buffer[256];
    int bufIdx = 0;

    OutputStream& output = (out != nullptr) ? *out : cout;

    // Parse the string character by character
    for (size_t i = 0; i <= rawInput.len(); i++) {
        // If we hit a space or the null terminator, finalize the token
        if (str[i] == ' ' || str[i] == '\0') {
            if (bufIdx > 0) {
                buffer[bufIdx] = '\0'; // Null terminate the buffer
                if (argCount < MAX_ARGS) {
                    args[argCount] = String(buffer);
                    argCount++;
                }
                bufIdx = 0; // Reset buffer for the next word
            }
        } else {
            // Prevent buffer overflow on giant words
            if (bufIdx < 255) {
                buffer[bufIdx++] = str[i];
            }
        }
    }

    if (argCount == 0) {
        return false;
    }

    const String cmd = args[0];

    if (is_shell_builtin(cmd))
    {
        return process_builtin(cmd, args, argCount, output);
    }
    output << "Command not found or external execution not yet supported: " << cmd.c_str() << "\r\n";

    return false;
}

void Shell::display_help(OutputStream &output) {
    output << "Type a command `help` for a list of available commands.\r\n";
}
