#pragma once
#include <stdint.h>
#include "String.hpp"

class OutputStream {
public:
    virtual ~OutputStream() = default;

    virtual OutputStream &operator<<(const char *str) = 0;
    virtual OutputStream &operator<<(const String &str) = 0;
    virtual OutputStream &operator<<(char c) = 0;
    virtual OutputStream &operator<<(int64_t num) = 0;
};
