#include <kernel/Shell.h>

#include "terminal.h"

bool is_shell_builtin(const String& cmd) {
    if (cmd == "help" || cmd == "clear" || cmd == "echo") {
        return true;
    }
    return false;
}

// Forward declaration of the built-in processor
bool process_builtin(const String& cmd, String* args, int argCount) {
    cout << "[Shell built-in command `" << cmd << "`]\r\n";
    if (argCount > 1) {
        cout << "Args:\r\n";
    }
    for (int i = 1; i < argCount; i++) {
        cout << "    - " << args[i] << "\r\n";
    }
    return false;
};

Shell &Shell::getInstance() {
    static Shell instance;
    return instance;
}

bool Shell::eval_user_input(const String &rawInput) {
    if (rawInput.empty()) {
        return false;
    }

    constexpr int MAX_ARGS = 16;
    String args[MAX_ARGS];
    int argCount = 0;

    const char* str = rawInput.c_str();
    char buffer[256];
    int bufIdx = 0;

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
        return process_builtin(cmd, args, argCount);
    }
    // Replace with your terminal output mechanism
    cout << "Command not found or external execution not yet supported: " << cmd.c_str() << "\r\n";

    return false;
}
