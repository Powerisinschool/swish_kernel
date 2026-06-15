#pragma once
#include <stdint.h>

struct registers_t {
    uint64_t x[30];
    uint64_t x30;
    uint64_t elr;
    uint64_t spsr;
    uint64_t padding;
};
