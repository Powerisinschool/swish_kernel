#pragma once
#include <stdint.h>
#include <compiler.h>

struct KERNEL_PACKED cpu_status {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rdx, rcx, rbx, rax;

    uint64_t int_no;
    uint64_t err_code;

    // Pushed automatically by the CPU
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

// This is the single C++ function all interrupts will funnel into
extern "C" void interrupt_handler(struct cpu_status* regs);
