#pragma once
#include <stdint.h>
#include "kernel/String.hpp"

struct flanterm_context;

class Terminal
{
public:
    void initialize(struct flanterm_context *context);

    Terminal &operator<<(const char *str);
    Terminal &operator<<(const String &str);
    Terminal &operator<<(char c);
    Terminal &operator<<(uint64_t num);
    Terminal &operator<<(int64_t num);
private:
    struct flanterm_context *ctx;
};

void k_print(const char *str);

extern Terminal cout;
