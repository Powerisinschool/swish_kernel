#include <arch/cpu.h>

void arch_halt_cpu() {
    __asm__ __volatile__("hlt");
}
