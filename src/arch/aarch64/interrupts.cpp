#include <arch/aarch64/interrupts.h>

#include "terminal.h"

extern "C" void interrupt_handler(registers_t *regs) {
    k_print("Exception occurred at address: ");
    cout << regs->elr << "\r\n";
}
