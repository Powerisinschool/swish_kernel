#pragma once
#include "Window.h"
#include "kernel/Shell.h"

class TerminalWindow : public Window, public OutputStream {
public:
    explicit TerminalWindow(size_t x, size_t y, size_t width, size_t height, const char *prompt_string = "user@kernel:~$ ");

    void inject_key(char c) final;

    TerminalWindow &operator<<(const char *str) final;
    TerminalWindow &operator<<(const String &str) final;
    TerminalWindow &operator<<(char c) final;
    TerminalWindow &operator<<(int64_t num) final;
private:
    const char *prompt;
    const size_t prompt_len;
    void print(char c);
    void print(const String &text);
};
