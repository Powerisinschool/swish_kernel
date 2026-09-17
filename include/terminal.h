#pragma once

#include "kernel/OutputStream.hpp"
#include "kernel/String.hpp"

struct flanterm_context;

class Terminal : public OutputStream
{
public:
    void initialize(flanterm_context *context);

    Terminal &operator<<(const char *str) final;
    Terminal &operator<<(const String &str) final;
    Terminal &operator<<(char c) final;
    Terminal &operator<<(int64_t num) final;
private:
    flanterm_context *ctx{};
};

void k_print(const char *str);

extern Terminal cout;
