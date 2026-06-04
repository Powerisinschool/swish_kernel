#include <stdint.h>
#include <terminal.h>

#include "kernel/String.hpp"

extern "C"
{
    #include <flanterm.h>
    size_t strlen(const char *str);
}

Terminal cout;

void Terminal::initialize(struct flanterm_context *context)
{
    ctx = context;
}

Terminal &Terminal::operator<<(const char *str)
{
    if (ctx && str)
    {
        flanterm_write(ctx, str, strlen(str));
    }
    return *this;
}

Terminal &Terminal::operator<<(const String &str) {
    if (ctx && !str.empty()) {
        flanterm_write(ctx, str.c_str(), str.len());
    }
    return *this;
}

Terminal &Terminal::operator<<(char c)
{
    if (ctx)
    {
        flanterm_write(ctx, &c, 1);
    }
    return *this;
}

Terminal &Terminal::operator<<(int64_t num)
{
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

void k_print(const char *str) {
    cout << str;
}
