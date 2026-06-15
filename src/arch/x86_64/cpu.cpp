#include <arch/hal/cpu.h>

#include "arch/x86_64/idt.h"
#include "drivers/pic.h"

void arch_initialize() {
    PIC::remap(0x20, 0x28);
    PIC::enable();
    IDT::initialize();
}

void arch_disable_interrupts() {
    __asm__ volatile("cli");
}

void arch_enable_interrupts() {
    __asm__ volatile("sti");
}

void arch_halt_cpu() {
    __asm__ volatile("hlt");
}
