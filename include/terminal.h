#pragma once

struct flanterm_context;

class Terminal
{
public:
    void initialize(struct flanterm_context *context);

    Terminal &operator<<(const char *str);
    Terminal &operator<<(char c);
    Terminal &operator<<(int64_t num);
private:
    struct flanterm_context *ctx;
};

extern Terminal cout;
